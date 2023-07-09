
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

Muestra una esfera texturada con una foto de la Tierra.

*/

class MyRender : public Renderer {
public:
    MyRender() :
        magic01(1.01f, 15, 25),
        magic02(2.0f, 20, 30),
        sph(1.0f, 15, 25)
    {};
    void setup(void);
    void render(void);
    void reshape(uint w, uint h);
    void update(uint ms);
    float returnAngel() { return this->angle; };

private:
    std::shared_ptr<GLMatrices> mats;
    std::shared_ptr<Program> program, pattern, noise;
    std::shared_ptr<Texture2D> earthTexture;
    std::shared_ptr<Texture2D> magicTexture[10];
    Sphere sph;
    Sphere magic01, magic02;
    void drawSatelite (Sphere&, float, float, vec3);
    void drawAtmosphere (Sphere&, float);
    void myGUI();
    std::shared_ptr<IntSliderWidget> tileFall, tilePP;
    std::shared_ptr<CheckBoxWidget> showTV, alpha, tvShader, shapeShader, colorShader;
    std::shared_ptr<FloatSliderWidget> brighter, xPP,yPP,zPP;
    std::shared_ptr<ListBoxWidget<>> colorMask, noiseTV;
    //std::shared_ptr<ListBoxWidget<>> shaderType;
    //GLint texUnitLoc0;
    float angle;
    GLint timePass;
    float time;
    float step;
    GLint smthGui;
    float smthPass;
    bool TVgui;
    float TVpass;
    float briGui;
    float briPass;
};

void MyRender::drawSatelite(Sphere& S, float A, float set = 1, vec3 pos = vec3(1.0f, 1.0f, 0.0f))
{
    mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    mats->translate(GLMatrices::MODEL_MATRIX, pos);
    mats->rotate(GLMatrices::MODEL_MATRIX, A, vec3(0.0f, 1.0f, 0.0f));
    //mats->rotate(GLMatrices::MODEL_MATRIX, angle, vec3(1.0f, 1.0f, 0.0f));
    //float scale = S.maxDimension()/set;
    mats->scale(GLMatrices::MODEL_MATRIX, vec3(set));
    S.render();
    mats->popMatrix(GLMatrices::MODEL_MATRIX);
}

void MyRender::drawAtmosphere (Sphere& S, float A)
{
    mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    mats->rotate(GLMatrices::MODEL_MATRIX, A, vec3(0.0f, 1.0f, 0.0f));
    S.render();
    mats->popMatrix(GLMatrices::MODEL_MATRIX);
}

void MyRender::myGUI() {
    auto panel = addPanel("Shaders");
    panel->setPosition(5, 50);
    panel->setSize(100, 850);

    //panel->addWidget(std::make_shared<Label>("Choose Shader"));

    //shaderType = std::make_shared<ListBoxWidget<>>("Shader type", std::vector<std::string> {"TV", "Pattern", "Noise"}, 0);
    //panel->addWidget(shaderType);

    //panel->addWidget(std::make_shared<Label>("TV Shader"));

    showTV = std::make_shared<CheckBoxWidget>("Old TV effect", false);
    panel->addWidget(showTV);

    tileFall = std::make_shared<IntSliderWidget>("Tiling", 1, 1, 6, program, "tile");
    panel->addWidget(tileFall);

    brighter = std::make_shared<FloatSliderWidget>("Brightness", 1, 0, 5);
    panel->addWidget(brighter);
    
    colorMask = std::make_shared<ListBoxWidget<>>("Color Masks", std::vector<std::string> {"none", "blue", "green", "red", "black&white"},
                                                            0, program, "swapColor");
    //colorMask->getValue().addListener([this](const int& i) {glUniform1i(swapVar, i); });
    panel->addWidget(colorMask);

    panel->addWidget(std::make_shared<Button>("Reset",
        [this]()
        {
            brighter->set(1);
            tileFall->set(1);
            showTV->set(false);
            colorMask->setSelected(0, true);
        }));

    //panel->addWidget(std::make_shared<Label>("Pattern Control Shader"));

    noiseTV = std::make_shared<ListBoxWidget<>>("Noise Type", std::vector<std::string>
    {"none", "hyperboloid", "bricks", "discoball",
        "floor", "color almaz", "leaves x<y<z reccommended",
        "caleidoscop", "my noise time dependant", "noise", "noise big1", "noise big2",
        "noise lines", "noise circles", "noise triangle"},
        0, program, "noiseIO");
    noiseTV->getValue().addListener([this](const int& i)
        {
            switch (i)
            {
            case 0:
                alpha->set(false);
                tilePP->set(5);
                xPP->set(0.25);
                yPP->set(0.35);
                zPP->set(0.45);
                break;
            case 1:
                alpha->set(true);
                tilePP->set(1);
                xPP->set(0.01);
                yPP->set(0.02);
                zPP->set(0.03);
                break;
            case 2:
                alpha->set(true);
                tilePP->set(10);
                xPP->set(0.95);
                yPP->set(0.95);
                zPP->set(0.95);
                break;
            case 3:
                alpha->set(true);
                tilePP->set(30);
                xPP->set(0.3);
                yPP->set(0.4);
                zPP->set(0.5);
                break;
            case 4:
                alpha->set(true);
                tilePP->set(10);
                xPP->set(0.95);
                yPP->set(0.3);
                zPP->set(0.2);
                break;
            case 5:
                alpha->set(true);
                tilePP->set(10);
                xPP->set(0.95);
                yPP->set(0.3);
                zPP->set(0.2);
                break;
            case 6:
                alpha->set(true);
                tilePP->set(20);
                xPP->set(0.2);
                yPP->set(0.3);
                zPP->set(0.4);
                break;
            case 9:
                alpha->set(true);
                tilePP->set(5);
                xPP->set(0.9);
                yPP->set(0.9);
                zPP->set(0.9);
                break;
            case 10:
                alpha->set(true);
                tilePP->set(30);
                xPP->set(0.8);
                yPP->set(0.8);
                zPP->set(0.8);
                break;
            case 11:
                alpha->set(true);
                tilePP->set(30);
                xPP->set(0.7);
                yPP->set(0.7);
                zPP->set(0.7);
                break;
            case 12:
                alpha->set(true);
                tilePP->set(30);
                xPP->set(0.25);
                yPP->set(0.35);
                zPP->set(0.9);
                break;
            case 13:
                alpha->set(true);
                tilePP->set(30);
                xPP->set(0.9);
                yPP->set(0.35);
                zPP->set(0.45);
                break;
            case 14:
                alpha->set(true);
                tilePP->set(30);
                xPP->set(0.25);
                yPP->set(0.9);
                zPP->set(0.45);
                break;
            default:
                alpha->set(true);
                tilePP->set(5);
                xPP->set(0.25);
                yPP->set(0.35);
                zPP->set(0.45);
                break;
            }
        });
    panel->addWidget(noiseTV);

    alpha = std::make_shared<CheckBoxWidget>("Use transparency", false, program, "alphaTime");
    panel->addWidget(alpha);

    tilePP = std::make_shared<IntSliderWidget>("PP Tiling", 5, 1, 30, program, "tilePPF");
    panel->addWidget(tilePP);

    xPP = std::make_shared<FloatSliderWidget>("X impact", 0.25, 0.001, 0.999, program, "xPPF");
    panel->addWidget(xPP);
    yPP = std::make_shared<FloatSliderWidget>("Y impact", 0.35, 0.001, 0.999, program, "yPPF");
    panel->addWidget(yPP);
    zPP = std::make_shared<FloatSliderWidget>("Z impact", 0.45, 0.001, 0.999, program, "zPPF");
    panel->addWidget(zPP);

    panel->addWidget(std::make_shared<Button>("Reset PP",
        [this]()
        {
            tilePP->set(5);
            alpha->set(false);
            xPP->set(0.25);
            yPP->set(0.35);
            zPP->set(0.45);
            noiseTV->setSelected(0, true);
        }));

    //panel->addWidget(std::make_shared<Label>("Color Control Shader"));

    //auto shaderColor = std::make_shared<RGBAColorWidget>("Color de fondo");
    //panel->addWidget(shaderColor);

}

void MyRender::setup() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    time = 0.0f;
    step = 0.004f;
    angle = 0.0f;
    smthGui=0;
    TVgui = false;
    briGui = 1;

    /* Este shader necesita las coordenadas de los vértices y sus
    coordenadas de textura */
    program = std::make_shared<Program>();
    program->addAttributeLocation(Mesh::VERTICES, "position");
    program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
    program->addAttributeLocation(Mesh::COLORS, "color");//atribute pointer vec3 = 3 floats

    mats = GLMatrices::build();
    program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);

    program->loadFiles("../p5-2/textureReplace");
    program->compile();
    /*
    pattern = std::make_shared<Program>();
    pattern->addAttributeLocation(Mesh::VERTICES, "position");
    pattern->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
    pattern->addAttributeLocation(Mesh::COLORS, "color");
    mats = GLMatrices::build();
    pattern->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
    pattern->loadFiles("../p5-2/pattern");
    pattern->compile();

    noise = std::make_shared<Program>();
    noise->addAttributeLocation(Mesh::VERTICES, "position");
    noise->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");
    noise->addAttributeLocation(Mesh::COLORS, "color");
    mats = GLMatrices::build();
    noise->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
    noise->loadFiles("../p5-2/noise");
    noise->compile();
    */
    // Localización de los uniform (unidad de textura)
    //texUnitLoc0 = program->getUniformLocation("texUnit0");
    smthPass = program->getUniformLocation("tile");
    TVpass = program->getUniformLocation("TVflag");
    briPass = program->getUniformLocation("bright");
    timePass = program->getUniformLocation("timeV");

    GLint texUnitLoc0 = program->getUniformLocation("texUnit0");
    program->use();
    glUniform1i(texUnitLoc0, 0);

    // Cargamos la nueva textura desde un fichero
    earthTexture = std::make_shared<Texture2D>();
    earthTexture->loadImage("../recursos/imagenes/tierra.jpg");
    //earthTexture->bind(GL_TEXTURE0);

    magicTexture[0] = std::make_shared<Texture2D>();
    magicTexture[0]->loadImage("../recursos/imagenes/mars.jpg");
    //magicTexture[0]->bind(GL_TEXTURE20);

    magicTexture[1] = std::make_shared<Texture2D>();
    magicTexture[1]->loadImage("../recursos/imagenes/venus.jpg");
    //magicTexture[1]->bind(GL_TEXTURE21);

    magicTexture[2] = std::make_shared<Texture2D>();
    magicTexture[2]->loadImage("../recursos/imagenes/venus_sky.jpg");
    //magicTexture[2]->bind(GL_TEXTURE22);

    magicTexture[3] = std::make_shared<Texture2D>();
    magicTexture[3]->loadImage("../recursos/imagenes/sky.jpg");
    //magicTexture[3]->bind(GL_TEXTURE23);

    magicTexture[4] = std::make_shared<Texture2D>();
    magicTexture[4]->loadImage("../recursos/imagenes/TexturesCom_ScratchedPaint3_overlay_S.tif");
    //magicTexture[4]->bind(GL_TEXTURE24);

    magicTexture[5] = std::make_shared<Texture2D>();
    magicTexture[5]->loadImage("../recursos/imagenes/mask_black.jpg");

    magicTexture[6] = std::make_shared<Texture2D>();
    magicTexture[6]->loadImage("../recursos/imagenes/texture.gif");

    magicTexture[7] = std::make_shared<Texture2D>();
    magicTexture[7]->loadImage("../recursos/imagenes/TexturesCom_Overlay_Dirt3_512_overlay.tif");

    //magicTexture[8] = std::make_shared<Texture2D>();
    //magicTexture[8]->loadImage("../recursos/imagenes/TexturesCom_ScratchedPaint3_overlay_S.tif");

    magicTexture[8] = std::make_shared<Texture2D>();
    magicTexture[8]->loadImage("../recursos/imagenes/TexturesCom_DirtyWall_overlay_S.tif");

    myGUI();
    App::getInstance().getWindow().showGUI(true);

    setCameraHandler(std::make_shared<OrbitCameraHandler>());
}

void MyRender::render() {
    // Comunicamos la unidad de textura al shader
    /*
    switch (shaderType->getSelected())
    {
    case 0:
        program->use();
        glUniform1i(texUnitLoc0, 0);
        break;
    case 1:
        pattern->use();
        break;
    case 2:
        noise->use();
        break;
    default:
        program->use();
        break;
    }
    glUniform1i(2, 0);
    */
    //glUniform1f(timePass, time);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

    //smthGui = tileFall->get();
    //glUniform1i(smthPass, smthGui);//WHYYYYYYYYYYYYYYYYY? why integer does not work?!
    TVgui = showTV->get();
    glUniform1f(TVpass, TVgui);
    briGui = brighter->get();
    glUniform1f(briPass, briGui);

    //set tile to 1 for planets
    glUniform1i(smthPass, 1);
    glActiveTexture(GL_TEXTURE0);
    //earth
    glFrontFace(GL_CCW);
    earthTexture->bind(GL_TEXTURE0);
    sph.render();
    //venus
    magicTexture[2]->bind(GL_TEXTURE0);
    drawSatelite(magic01, -angle * 25, 0.38f);
    //mars
    magicTexture[0]->bind(GL_TEXTURE0);
    drawSatelite(magic01, -angle * 15, 0.25f, vec3(0.0f,-1.0f,1.0f));
    //set tile to user chosen
    smthGui = tileFall->get();
    glUniform1i(smthPass, smthGui);
    
    //glEnable(GL_CULL_FACE);

    glUniform1i(smthPass, smthGui+1);
    //cheese
    magicTexture[6]->bind(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    drawSatelite(magic01, angle * 20, 0.4f);
    smthGui = tileFall->get();
    glUniform1i(smthPass, smthGui);
    //magic01.render();

    glEnable(GL_BLEND);
    //sky
    magicTexture[3]->bind(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    drawAtmosphere(magic01, -angle*2);
    //magic02.render();
    //magic ball
    //mars sky
    magicTexture[8]->bind(GL_TEXTURE0);
    drawSatelite(magic01, -angle * 15, 0.4f, vec3(0.0f, -1.0f, 1.0f));
    //mars sky end
    magicTexture[7]->bind(GL_TEXTURE0);
    drawAtmosphere(magic02, angle*3);

    glEnable(GL_CULL_FACE);
    magicTexture[4]->bind(GL_TEXTURE0);
    mats->pushMatrix(GLMatrices::MODEL_MATRIX);
    mats->scale(GLMatrices::MODEL_MATRIX, vec3(0.98f));
    drawAtmosphere(magic02, -angle * 3);
    mats->popMatrix(GLMatrices::MODEL_MATRIX);
    glDisable(GL_CULL_FACE);

    glDisable(GL_BLEND);

    CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
    glViewport(0, 0, w, h);
    mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

void MyRender::update(uint ms) {
    time += step;
    if (time <= 0.0|| 1.0 <= time)
        step = -step;
    glUniform1f(timePass, time);
    angle += glm::radians(45.0f) * ms / 10000.0f;
    if (angle > TWOPIf)
        angle -= TWOPIf;
    mats->setMatrix(GLMatrices::MODEL_MATRIX,
        glm::rotate(glm::mat4(1.0f), angle, vec3(0.1, 1.0, -0.1)));
}

int main(int argc, char* argv[]) {
    App& myApp = App::getInstance();
    myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
        PGUPV::MULTISAMPLE);
    myApp.getWindow().setRenderer(std::make_shared<MyRender>());
    return myApp.run();
}
