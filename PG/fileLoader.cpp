#include <sstream>

#include "fileLoader.h"
#include "assimpWrapper.h"
#include "scene.h"
#include "log.h"
#include "utils.h"
#include "describeScenegraph.h"
#include "findNodeByName.h"
#include "properties.h"
#include "bindableTexture.h"

#include "texture2D.h"

using PGUPV::Scene;
using PGUPV::AssimpWrapper;
using PGUPV::FileLoader;
using PGUPV::Properties;
using PGUPV::FindNodeByName;
using PGUPV::Material;
using PGUPV::BindableTexture;
using PGUPV::Texture2D;

#include "nodeVisitor.h"

using PGUPV::NodeVisitor;
using PGUPV::Node;
using PGUPV::Geode;



struct PGMATProperty {
	enum class NodeType {
		Node, Material, Unkwnown
	};

	NodeType type;
	std::string name;
	std::string propertyName;
	int index;

	PGMATProperty(NodeType t, const std::string &name, const std::string &propertyName, int index) :
		type(t), name(name), propertyName(propertyName), index(index) {};

	PGMATProperty(NodeType t, const std::string &name, const std::string &propertyName) :
		PGMATProperty(t, name, propertyName, -1) {};

	std::string to_string();
	static PGMATProperty from_string(const std::string &str);
};

std::string PGMATProperty::to_string() {
	std::ostringstream os;

	os << (type == NodeType::Node ? "n_" : "m_");
	os << name << "_" << propertyName;
	if (index >= 0) os << "[" << index << "]";
	return os.str();
}

PGMATProperty PGMATProperty::from_string(const std::string &str) {
	PGMATProperty res(PGMATProperty::NodeType::Unkwnown, "", "");

	auto last_underscore = str.find_last_of('_');
	if (last_underscore == str.npos) {
		return res;
	}

	if (PGUPV::starts_with(str, "n_"))
		res.type = PGMATProperty::NodeType::Node;
	else if (PGUPV::starts_with(str, "m_"))
		res.type = PGMATProperty::NodeType::Material;
	else
		return res;

	res.name = str.substr(2, last_underscore - 2);

	auto bracket = str.find_last_of('[');
	if (bracket == str.npos) {
		res.propertyName = str.substr(last_underscore + 1);
	}
	else {
		res.propertyName = str.substr(last_underscore + 1, bracket - last_underscore - 1);
		auto indexStr = str.substr(bracket + 1, str.size() - bracket - 1 - 1);
		res.index = std::stoi(indexStr);
	}
	return res;
}

class VisitNodes : public NodeVisitor {
public:
	VisitNodes(std::ostream &os) :
		os(os) {};
	void apply(Node &node) override {
		PGMATProperty pgmat(PGMATProperty::NodeType::Node, node.getName(), "visible");
		os << "# " << pgmat.to_string() << " = false\n";
		traverse(node);
	}
	//void apply(Group &group) override;
	//void apply(Transform &transform) override;
	//void apply(Geode &geode) override {
	  //ode.getModel().getMesh(0).

private:
	std::ostream &os;
};



static void textureTemplate(std::ostream &f, int /*matId*/, Material &m, Material::TextureType t) {
	auto count = m.getTextureCount(t);
	std::string typeName;
	switch (t) {
	case Material::TextureType::DIFFUSE_TUNIT:
		typeName = "diffusemap";
		break;
	case Material::TextureType::SPECULAR_TUNIT:
		typeName = "specularmap";
		break;
	case Material::TextureType::HEIGHTMAP_TUNIT:
		typeName = "heightmap";
		break;
	case Material::TextureType::AMBIENT_TUNIT:
		typeName = "ambientmap";
		break;
	case Material::TextureType::NORMALMAP_TUNIT:
		typeName = "normalmap";
		break;
	case Material::TextureType::OPACITYMAP_TUNIT:
		typeName = "opacitymap";
		break;
	default:
		break;
	}

	PGMATProperty pgmat{ PGMATProperty::NodeType::Material, m.getName(), typeName, 0 };

	if (!count) {
		f << "# " << pgmat.to_string() << " = <texture-filename>\n";
	}
	else {
		for (unsigned int j = 0; j < count; j++) {
			pgmat.index = j;
			f << "# " << pgmat.to_string() << " = " << m.getTexture(t + j)->getName() << "\n";
		}
	}
}

static void templateMaterial(std::ofstream &f, int matId, Material &m) {
	PGMATProperty prop(PGMATProperty::NodeType::Material, m.getName(), "");

	prop.propertyName = "ambient";
	f << "# " << prop.to_string() << " = " << PGUPV::to_string(m.getAmbient()) << "\n";
	prop.propertyName = "diffuse";
	f << "# " << prop.to_string() << " = " << PGUPV::to_string(m.getDiffuse()) << "\n";
	prop.propertyName = "specular";
	f << "# " << prop.to_string() << " = " << PGUPV::to_string(m.getSpecular()) << "\n";
	prop.propertyName = "emissive";
	f << "# " << prop.to_string() << " = " << PGUPV::to_string(m.getEmissive()) << "\n";
	prop.propertyName = "shininess";
	f << "# " << prop.to_string() << " = " << PGUPV::to_string(m.getShininess()) << "\n";

	textureTemplate(f, matId, m, Material::DIFFUSE_TUNIT);
	textureTemplate(f, matId, m, Material::SPECULAR_TUNIT);
	textureTemplate(f, matId, m, Material::NORMALMAP_TUNIT);
	textureTemplate(f, matId, m, Material::HEIGHTMAP_TUNIT);
	textureTemplate(f, matId, m, Material::OPACITYMAP_TUNIT);
	textureTemplate(f, matId, m, Material::AMBIENT_TUNIT);
}

static void createTemplatePGMAT(const std::string &path, std::shared_ptr<Scene> scene) {
	std::ofstream f(path);

	if (!f) return;

	f << "# NODES\n";
	VisitNodes vn(f);
	scene->getRoot()->accept(vn);

	f << "\n\n# MATERIALS\n";
	for (unsigned int i = 0; i < scene->getNumMaterials(); i++) {
		auto m = scene->getMaterial(i);
		templateMaterial(f, i, *m);
		f << "\n";
	}
}


static void loadPGMAT(const std::string &path, std::shared_ptr<Scene> scene) {
	Properties props;

	props.load(path);
	for (size_t i = 0; i < props.numProperties(); i++) {
		auto p = props.getKey(i);
		auto pgmat = PGMATProperty::from_string(p);
		switch (pgmat.type) {
		case PGMATProperty::NodeType::Node:
		{
			if (pgmat.propertyName == "visible") {
				PGUPV::FindNodeByName find(pgmat.name);
				scene->getRoot()->accept(find);
				if (!find.getResults().size()) {
					ERR("No se ha podido encontrar el nodo " + pgmat.name);
					continue;
				}
				bool visible;
				props.value(p, visible);
				for (auto n : find.getResults()) {
					n->setVisible(visible);
				}
			}
		}
		break;
		case PGMATProperty::NodeType::Material:
		{
			std::string value;
			props.value(p, value);

			auto material = scene->getMaterial(pgmat.name);
			if (!material) {
				ERR("El material indicado en el fichero " + path + "no existe");
				continue;
			}

			if (pgmat.propertyName == "ambient") {
				material->setAmbient(PGUPV::to_vec<glm::vec4>(value));
				continue;
			}
			else if (pgmat.propertyName == "diffuse") {
				material->setDiffuse(PGUPV::to_vec<glm::vec4>(value));
				continue;
			}
			else if (pgmat.propertyName == "specular") {
				material->setSpecular(PGUPV::to_vec<glm::vec4>(value));
				continue;
			}
			else if (pgmat.propertyName == "emissive") {
				material->setEmissive(PGUPV::to_vec<glm::vec4>(value));
				continue;
			}
			else if (pgmat.propertyName == "shininess") {
				material->setShininess(std::stof(value));
				continue;
			}

			// Es una textura
			value = PGUPV::getDirectory(path) + value;

			auto theTexture = std::make_shared<PGUPV::Texture2D>(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			theTexture->loadImage(value);
			theTexture->generateMipmap();

			if (pgmat.propertyName == "diffusemap") {
				material->setDiffuseTexture(theTexture, pgmat.index);
			}
			else if (pgmat.propertyName == "specularmap") {
				material->setSpecularTexture(theTexture, pgmat.index);
			}
			else if (pgmat.propertyName == "normalmap") {
				material->setNormalMapTexture(theTexture, pgmat.index);
			}
			else if (pgmat.propertyName == "heightmap") {
				material->setHeightMapTexture(theTexture, pgmat.index);
			}
			else if (pgmat.propertyName == "opacitymap") {
				material->setOpacitytMapTexture(theTexture, pgmat.index);
			}
			else if (pgmat.propertyName == "ambientmap") {
				material->setAmbientMapTexture(theTexture, pgmat.index);
			}
			else {
				ERR("Unknown entry: " + p);
			}
		}
		break;
		default:
			ERR("Sintaxis incorrecta en la propiedad: " + p);
		}
	}
}


std::shared_ptr<Scene> FileLoader::load(const std::string &path, AssimpWrapper::LoadOptions options) {
	AssimpWrapper loader;
	auto scene = loader.load(path, options);
	if (!scene) {
		ERRT("Error cargando el fichero " + path);
	}

	std::ostringstream os;
	PGUPV::DescribeScenegraph describer(os);
	scene->getRoot()->accept(describer);
	INFO("Grafo de escena:\n" + os.str());


	auto extraMaterialProps = PGUPV::removeExtension(path) + ".pgmat";
	if (!fileExists(extraMaterialProps)) {
		// No hay un fichero extra de materiales: crear una plantilla
		createTemplatePGMAT(extraMaterialProps, scene);
	}
	else {
		// Hay un fichero extra de materiales: cargarlo
		loadPGMAT(extraMaterialProps, scene);
	}

	return scene;
}

std::vector<PGUPV::ExportFileFormat> FileLoader::getSupportedExportFileFormats() 
{
	AssimpWrapper assimp;
	return assimp.listSupportedExportFormat();
}


bool FileLoader::save(const std::string &path, const std::string &id, std::shared_ptr<Scene> scene) {
	AssimpWrapper assimp;
	INFO("Guardando el fichero " + path + " como " + id);

	return assimp.save(path, id, scene);
}
