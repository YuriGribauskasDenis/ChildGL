
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

Simulando un volumen de recorte cilíndrico con un shader de geometría

*/

#define INITIAL_RADIUS 0.25f
#define INITIAL_HEIGHT 0.5f
#define INITIAL_PROPORTION 0.2f
#define INITIAL_AXIS glm::vec3(0.0f, 1.0f, 0.0f)

class MyRender : public Renderer {
public:
    MyRender()
        :
        cylinderPosWCS(0.8f, 0.8f, 0.0f), // Posición en el S.C. del mundo
        boxPosWCS(0.2f, 0.25f, 0.6f),
        cylinderModel(1.0f, 1.0f, 1.0f, 2),
        boxModel(1.0f),
    // Este objeto interpola linealmente entre las dos primeras variables,
    // la tercera es la duración de un ciclo y la cuarta el tipo de
    // movimiento
    // Se usa para calcular la posición del cilindro en cada momento.
    li(cylinderPosWCS, vec3(-0.6, -0.4, 0), 9.0, PING_PONG),
    bo(boxPosWCS, vec3(0.2f, 0.25f, -0.6f), 5.0, PING_PONG)
    {};
  void setup(void) override;
  void render(void) override;
  void reshape(uint w, uint h) override;
  void update(uint ms) override;

private:
  std::shared_ptr<GLMatrices> mats;
  Axes axes;
  std::shared_ptr<Program> program;
  GLint lightPosLoc;
  vec4 lightPosWCS;
  GLint cylinderPosLoc;
  vec3 cylinderPosWCS; // Posición de la base del cilindro en el S.C. del mundo
  vec3 boxPosWCS;
  GLint boxPosLoc;
  mat3 boxMat;
  GLint matLocCyl, matLocBox;

  Cylinder cylinderModel;
  Box boxModel;
  std::shared_ptr<Scene> bunny;
  LinearInterpolator<vec3> li;
  LinearInterpolator<vec3> bo;

  std::shared_ptr<CheckBoxWidget> showBox, showQuadr;
  std::shared_ptr<FloatSliderWidget> cylinderHeight, cylinderRadius, boxSizeX, boxSizeY, boxSizeZ, quadrSize;
  std::shared_ptr<DirectionWidget> cylinderAxis, boxAxis; // Eje del cilindro en el S.C. del mundo
  void buildGUI();
};

void MyRender::update(uint ms) {
  static uint total = 0;

  total += ms;
  cylinderPosWCS = li.interpolate(total / 1000.0);
  boxPosWCS = bo.interpolate(total / 1000.0);
}

void MyRender::setup() {
  glClearColor(1.f, 1.f, 1.f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  // Inicialización de variables de estado
  // Posición de la luz en el S.C. del mundo
  lightPosWCS = vec4(2.f, 2.f, 2.f, 1.f);

  bunny = FileLoader::load("../recursos/modelos/bunny.obj");

  /* Este shader se encarga de calcular la iluminación (sólo componente
   * difusa)*/
  program = std::make_shared<Program>();
  program->addAttributeLocation(Mesh::VERTICES, "position");
  program->addAttributeLocation(Mesh::NORMALS, "normal");

  mats = GLMatrices::build();
  program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
  program->loadFiles("../p7/p7");
  program->compile();

  // Posiciones de las variables uniform
  lightPosLoc = program->getUniformLocation("lightpos");
  cylinderPosLoc = program->getUniformLocation("cylinderPos");
  boxPosLoc = program->getUniformLocation("boxPos");
  matLocBox = program->getUniformLocation("boxMatPass");
  matLocCyl = program->getUniformLocation("cylMatPass");

  // Color del objeto
  program->use();
  GLint diffuseLoc = program->getUniformLocation("diffuseColor");
  glUniform4f(diffuseLoc, 0.8f, 0.8f, 0.8f, 1.0f);

  setCameraHandler(std::make_shared<OrbitCameraHandler>());

  buildGUI();
  App::getInstance().getWindow().showGUI();
}

void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 viewMatrix = getCamera().getViewMatrix();
  mats->setMatrix(GLMatrices::VIEW_MATRIX, viewMatrix);

  program->use();
  // Posición de la fuente en el S.C. de la cámara
  vec4 lp = viewMatrix * lightPosWCS;
  glUniform3f(lightPosLoc, lp.x, lp.y, lp.z);

  // Posición de la base del cilindro en el espacio del mundo
  glUniform3fv(cylinderPosLoc, 1, &cylinderPosWCS.x);
  glUniform3fv(boxPosLoc, 1, &boxPosWCS.x);

  //not very elegant but sending move matrices of two objects
  auto t = cylinderAxis->get();
  auto yp1 = t.y + 1.0f;
  boxMat = mat3(
      t.y + (t.z * t.z / yp1), -t.x, -t.z * t.x / yp1,
      t.x, t.y, t.z,
      -t.z * t.x / yp1, -t.z, t.y + (t.x * t.x / yp1));
  //boxMat = mat3(glm::vec3(tmpMat[0]), glm::vec3(tmpMat[1]), glm::vec3(tmpMat[2]));
  glUniformMatrix3fv(matLocCyl, 1, GL_FALSE, &boxMat[0][0]);//glm::value_ptr()?
  t = boxAxis->get();
  yp1 = t.y + 1.0f;
  boxMat = mat3(
      t.y + (t.z * t.z / yp1), -t.x, -t.z * t.x / yp1,
      t.x, t.y, t.z,
      -t.z * t.x / yp1, -t.z, t.y + (t.x * t.x / yp1));
  glUniformMatrix3fv(matLocBox, 1, GL_FALSE, &boxMat[0][0]);

  // Dibujando el objeto
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.3f, 0.3f, 0.0f);
  mats->scale(GLMatrices::MODEL_MATRIX, vec3(1.2f / bunny->maxDimension()));
  mats->translate(GLMatrices::MODEL_MATRIX, -bunny->center());
  bunny->render();
  mats->popMatrix(GLMatrices::MODEL_MATRIX);

  // Un shader sencillo para dibujar el color definido por cada vértice
  ConstantIllumProgram::use();

  // Dibujamos los ejes de coordenadas
  axes.render();

  // Dibujamos el cilindro en alámbrico
  mats->pushMatrix(GLMatrices::MODEL_MATRIX);
  mats->translate(GLMatrices::MODEL_MATRIX, cylinderPosWCS);
  GLint prevMode;
  glGetIntegerv(GL_POLYGON_MODE, &prevMode);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  t = cylinderAxis->get();
  yp1 = t.y + 1.0f;
  mats->multMatrix(GLMatrices::MODEL_MATRIX, glm::mat4(
    t.y + (t.z * t.z / yp1), -t.x, -t.z * t.x / yp1, 0.0f,
    t.x, t.y, t.z, 0.0f,
    -t.z*t.x / yp1, -t.z, t.y + (t.x*t.x / yp1), 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  ));
  vec3 cylScale(cylinderRadius->get());
  cylScale.y = cylinderHeight->get();
  mats->scale(GLMatrices::MODEL_MATRIX, cylScale);
  mats->translate(GLMatrices::MODEL_MATRIX, 0.0f, 0.5f, 0.0f);
  cylinderModel.render();
  glPolygonMode(GL_FRONT_AND_BACK, prevMode);
  mats->popMatrix(GLMatrices::MODEL_MATRIX);
  
  if (showBox->get())
  {
      mats->pushMatrix(GLMatrices::MODEL_MATRIX);
      mats->translate(GLMatrices::MODEL_MATRIX, boxPosWCS);
      GLint prevModeB;
      glGetIntegerv(GL_POLYGON_MODE, &prevModeB);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      t = boxAxis->get();
      yp1 = t.y + 1.0f;
      mats->multMatrix(GLMatrices::MODEL_MATRIX, mat4(
          t.y + (t.z * t.z / yp1), -t.x, -t.z * t.x / yp1, 0.0f,
          t.x, t.y, t.z, 0.0f,
          -t.z * t.x / yp1, -t.z, t.y + (t.x * t.x / yp1), 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f));
      vec3 boxScale(vec3(1.0f));
      boxScale.x = boxSizeX->get();
      boxScale.y = boxSizeY->get();
      boxScale.z = boxSizeZ->get();
      mats->scale(GLMatrices::MODEL_MATRIX, boxScale);
      mats->translate(GLMatrices::MODEL_MATRIX, 0.0f, 0.5f, 0.0f);
      boxModel.render();
      glPolygonMode(GL_FRONT_AND_BACK, prevMode);
      mats->popMatrix(GLMatrices::MODEL_MATRIX);
  }

  CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
  glViewport(0, 0, w, h);
  if (h == 0)
    h = 1;
  float ar = (float)w / h;
  mats->setMatrix(GLMatrices::PROJ_MATRIX,
    glm::perspective(glm::radians(60.0f), ar, .1f, 100.0f));
  mats->loadIdentity(GLMatrices::MODEL_MATRIX);
}

void MyRender::buildGUI() {
  // Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
  auto panel = addPanel("Cilindro");

  // Podemos darle un tamaño y una posición, aunque no es necesario
  panel->setPosition(5, 50);
  panel->setSize(180, 880);

  showQuadr = std::make_shared<CheckBoxWidget>("Show Quadr", false, program, "quadrFlag");
  panel->addWidget(showQuadr);

  quadrSize = std::make_shared<FloatSliderWidget>("Quadr Size", 0.025f, 0.01f, 0.05f,
      program, "dist");
  panel->addWidget(quadrSize);

  cylinderRadius = std::make_shared<FloatSliderWidget>("Radio", INITIAL_RADIUS, 0.001f, 2.0f,
    program, "cylinderRadius");
  panel->addWidget(cylinderRadius);

  cylinderHeight = std::make_shared<FloatSliderWidget>("Altura", INITIAL_HEIGHT, 0.001f, 2.0f,
    program, "cylinderHeight");
  panel->addWidget(cylinderHeight);

  cylinderAxis = std::make_shared<DirectionWidget>("Eje", INITIAL_AXIS, program, "cylinderAxis");
  cylinderAxis->setCamera(getCameraHandler()->getCameraPtr());
  panel->addWidget(cylinderAxis);

  showBox = std::make_shared<CheckBoxWidget>("Use Box", false, program, "boxFlag");
  panel->addWidget(showBox);

  boxSizeX = std::make_shared<FloatSliderWidget>("box X", INITIAL_PROPORTION, 0.001f, 2.0f,
      program, "box_X");
  panel->addWidget(boxSizeX);

  boxSizeZ = std::make_shared<FloatSliderWidget>("box Z", INITIAL_PROPORTION, 0.001f, 2.0f,
      program, "box_Z");
  panel->addWidget(boxSizeZ);

  boxSizeY = std::make_shared<FloatSliderWidget>("box Y", INITIAL_PROPORTION, 0.001f, 2.0f,
      program, "box_Y");
  panel->addWidget(boxSizeY);

  boxAxis = std::make_shared<DirectionWidget>("AXE", INITIAL_AXIS, program, "boxAxis");
  boxAxis->setCamera(getCameraHandler()->getCameraPtr());
  panel->addWidget(boxAxis);

  panel->addWidget(std::make_shared<Button>("Reset",
      [this]()
      {
          cylinderRadius->set(INITIAL_RADIUS);
          cylinderHeight->set(INITIAL_HEIGHT);
          cylinderAxis->set(INITIAL_AXIS);
          showBox->set(false);
          boxSizeX->set(INITIAL_PROPORTION);
          boxSizeZ->set(INITIAL_PROPORTION);
          boxSizeY->set(INITIAL_PROPORTION);
          boxAxis->set(INITIAL_AXIS);
          quadrSize->set(0.025f);
      }));

}

int main(int argc, char *argv[]) {
  App &myApp = App::getInstance();
  myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
    PGUPV::MULTISAMPLE);
  myApp.getWindow().setRenderer(std::make_shared<MyRender>());
  return myApp.run();
}
