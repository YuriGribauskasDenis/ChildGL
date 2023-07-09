
#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"
#include "GUI3.h"

using namespace PGUPV;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using std::vector;

/*

Visualizador de terreno usando shaders de teselación.

*/

class MyRender : public Renderer {
public:
    MyRender() : program(std::make_shared<Program>()),
        lightPosWCS(5000.0f, 5000.0f, 5000.0f),
        bo(lightPosWCS, vec3(-5000.0f, 5000.0f, -5000.0f), 5.0, PING_PONG)
    {};
    void setup(void) override;
    void render(void) override;
    void reshape(uint w, uint h) override;
    void update(uint ms) override;

private:
    void buildGUI();
    std::shared_ptr<Program> program;
    std::shared_ptr<GLMatrices> mats;
    Model model;
    Texture2D heightMap;
    //GLint lightPosLoc;
    //vec4 lightPosWCS;
    std::shared_ptr<FloatSliderWidget> depthSize;
    std::shared_ptr<DirectionWidget> lightDirectoin;
    std::shared_ptr<CheckBoxWidget> showPatch;
    Texture2D normalMap;
    Texture1D colorMap;
    GLint lightPosLoc;
    vec3 lightPosWCS;
    Box boxModel;
    std::shared_ptr<FloatSliderWidget> eat, eati, emit, emiti;
    LinearInterpolator<vec3> bo;
    //void updateLightPosition(const glm::mat4& viewMatrix);
    std::shared_ptr<ListBoxWidget<>> fragmentType;
};

void MyRender::buildGUI() {
    auto panel = addPanel("Lights");
    panel->setPosition(5, 5);
    panel->setSize(280, 550);
    panel->addWidget(std::make_shared<Label>("Post Process type"));
    fragmentType = std::make_shared<ListBoxWidget<>>("PP", std::vector<std::string> {
        "None", "Fog", "Sun", "light"}, 0, program, "PP");
    panel->addWidget(fragmentType);
    
    panel->addWidget(std::make_shared<Label>("Fog Coef"));
    eat = std::make_shared<FloatSliderWidget>("absorb", 6, 1, 100, program, "eat");
    panel->addWidget(eat);
    eati = std::make_shared<FloatSliderWidget>("intencity", 0.025, 0.001, 2, program, "eati");
    panel->addWidget(eati);
    emit = std::make_shared<FloatSliderWidget>("emit", 40, 1, 100, program, "emit");
    panel->addWidget(emit);
    emiti = std::make_shared<FloatSliderWidget>("intencity2", 0.045, 0.001, 2, program, "emiti");
    panel->addWidget(emiti);
    
    panel->addWidget(std::make_shared<Label>("Control Light"));
    lightDirectoin = std::make_shared<DirectionWidget>("LightDir", glm::vec3(0.0, 50.0, 0.0f), program, "lightDirUI");
    lightDirectoin->getValue().addListener([this](const vec3 &dir)
    {
        //
    });
    panel->addWidget(lightDirectoin);
    panel->addWidget(std::make_shared<Label>("Hide polygons outside camera"));
    showPatch = std::make_shared<CheckBoxWidget>("Patch", false, program, "patchFlag");
    panel->addWidget(showPatch);
    panel->addWidget(std::make_shared<Label>("Displacement rate"));
    depthSize = std::make_shared<FloatSliderWidget>("depth", 1.0f, 0.0f, 2.0f, program, "depth");
    panel->addWidget(depthSize);

}

void MyRender::update(uint ms)
{
    static uint total = 0;
    total += ms;
    lightPosWCS = bo.interpolate(total / 1000.0);
}

void MyRender::setup() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    auto m = std::make_shared<Mesh>();
    std::vector<glm::vec3> vs;
    vs.push_back(glm::vec3(-32.0, 0.0, -32.0));
    vs.push_back(glm::vec3(32.0, 0.0, -32.0));
    vs.push_back(glm::vec3(-32.0, 0.0, 32.0));
    vs.push_back(glm::vec3(32.0, 0.0, 32.0));
    m->addVertices(vs);

    std::vector<glm::vec2> tc;
    tc.push_back(glm::vec2(0.0f, 0.0f));
    tc.push_back(glm::vec2(1.0f, 0.0f));
    tc.push_back(glm::vec2(0.0f, 1.0f));
    tc.push_back(glm::vec2(1.0f, 1.0f));
    m->addTexCoord(0, tc);

    DrawArraysInstanced* dc = new DrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
    dc->setVerticesPerPatch(4);
    m->addDrawCommand(dc);
    model.addMesh(m);

    mats = GLMatrices::build();

    program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
    program->addAttributeLocation(Mesh::VERTICES, "position");
    program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
    program->loadFiles("../p8/shader");
    program->compile();
    lightPosLoc = program->getUniformLocation("light_pos");
    program->use();


    GLint normalMapLoc = program->getUniformLocation("normalMap");
    glUniform1i(normalMapLoc, 1);
    normalMap.loadImage("../recursos/imagenes/NormalMap.png");
    //normalMap.bind(GL_TEXTURE1);

    GLint texUnitHeightMapLoc = program->getUniformLocation("texUnitHeightMap");
    glUniform1i(texUnitHeightMapLoc, 0);
    // Cargamos la nueva textura desde un fichero
    heightMap.loadImage("../recursos/imagenes/heightmap.png");
    heightMap.bind(GL_TEXTURE0);
    /*
    GLint normalMapLoc = program->getUniformLocation("normalMap");
    glUniform1i(normalMapLoc, 1);
    normalMap.loadImage("../recursos/imagenes/NormalMap.png");
    normalMap.bind(GL_TEXTURE1);
    */
    GLint colorGradientLoc = program->getUniformLocation("colorGradient");
    glUniform1i(colorGradientLoc, 2);
    colorMap.loadImage("../recursos/imagenes/colorScaleHM.png");
    colorMap.bind(GL_TEXTURE2);
    //glActiveTexture(GL_TEXTURE2);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    //auto camera = std::make_shared<WalkCameraHandler>(15.0f, glm::vec4(0.0f, 85.0f, 0.0f, 1.0f), 0.0f, 0.0f); // Altura inicial de la cámara
    auto camera = std::make_shared<WalkCameraHandler>(10.0f);
    camera->setWalkSpeed(1.0f);
    camera->setVerticalSpeed(0.5f);
    setCameraHandler(camera);

    buildGUI();
    App::getInstance().getWindow().showGUI(true);
}

void MyRender::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightDirectoin->setCamera(getCameraHandler()->getCameraPtr());
    glUniform3fv(lightPosLoc, 1, &lightPosWCS.x);
    mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());
    program->use();
    normalMap.bind(GL_TEXTURE1);
    model.render();

    if (getManufacturer() == PGUPV::Manufacturer::AMD)
        glUseProgram(0);

    CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
    glViewport(0, 0, w, h);
    if (h == 0)
        h = 1;
    float ar = (float)w / h;
    mats->setMatrix(GLMatrices::PROJ_MATRIX,
        glm::perspective(glm::radians(60.0f), ar, 0.1f, 1000.0f));
}

int main(int argc, char* argv[]) {
    App& myApp = App::getInstance();
    myApp.setInitWindowSize(800, 600);
    myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
    myApp.getWindow().setRenderer(std::make_shared<MyRender>());
    return myApp.run();
}