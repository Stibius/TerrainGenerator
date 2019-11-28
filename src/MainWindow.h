#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QImage>
#include <QPixmap>

#include <string>

#include "MyGLWidget.h"
#include "Heightmap.h"
#include "HeightmapGenerator.h"
#include "Mesh.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:

    explicit MainWindow(QWidget* parent = nullptr);

	MainWindow(const MainWindow& other) = delete;

	MainWindow(MainWindow&& other) = delete;

	MainWindow& operator=(const MainWindow& other) = delete;

	MainWindow& operator=(MainWindow&& other) = delete;
    
	~MainWindow() override;

	MyGLWidget& getGLWidget() const;

private slots:

    void on_actionQuit_triggered();

    void on_actionOpenHeightmap_triggered();

    void on_actionSaveHeightmap_triggered();

    void on_diamondIterationsSpinBox_valueChanged(int arg1);

    void on_generateDiamondPushButton_clicked();

    void on_generateCirclesPushButton_clicked();

    void on_generatePerlinPushButton_clicked();

    void on_generateFaultPushButton_clicked();

	void on_heightExpSpinBox_valueChanged(double arg1);

    void on_actionCreateMesh_triggered();

    void on_actionOpenMesh_triggered();

    void on_actionSaveMesh_triggered();

    void on_ambientPushButton_clicked();

	void on_diffusePushButton_clicked();

	void on_specularPushButton_clicked();

	void on_maxYSpinBox_valueChanged(double arg1);

    void on_textureCheckBox_clicked(bool checked);

    void on_textureScaleXSpinBox_valueChanged(double arg1);

	void on_textureScaleYSpinBox_valueChanged(double arg1);

	void on_textureScaleZSpinBox_valueChanged(double arg1);

    void on_fillRadioButton_clicked(bool checked);

    void on_linesRadioButton_clicked(bool checked);

    void on_pointsRadioButton_clicked(bool checked);

    void on_actionOpenTexture_triggered();

    void on_lightXSpinBox_valueChanged(double arg1);

    void on_lightYSpinBox_valueChanged(double arg1);

    void on_lightZSpinBox_valueChanged(double arg1);

    void on_lightColorPushButton_clicked();

	void on_addMatPushButton_clicked();

	void on_removeMatPushButton_clicked();

	void on_matUpPushButton_clicked();

	void on_matDownPushButton_clicked();

	void on_materialHeightBar_selectedSectionChanged(int index);

	void on_materialHeightBar_sectionHeightsChanged();

    void on_ambientRedSpinBox_valueChanged(int arg1);

	void on_ambientGreenSpinBox_valueChanged(int arg1);

	void on_ambientBlueSpinBox_valueChanged(int arg1);

	void on_diffuseRedSpinBox_valueChanged(int arg1);

	void on_diffuseGreenSpinBox_valueChanged(int arg1);

	void on_diffuseBlueSpinBox_valueChanged(int arg1);

	void on_specularRedSpinBox_valueChanged(int arg1);

	void on_specularGreenSpinBox_valueChanged(int arg1);

	void on_specularBlueSpinBox_valueChanged(int arg1);

    void on_shininessSpinBox_valueChanged(int arg1);

    void on_shadowsCheckBox_toggled(bool checked);

    void on_poissonSpreadSpinBox_valueChanged(int arg1);

	void on_shadowBiasSpinBox_valueChanged(double arg1);

	void on_myGLWidget_renderingFinished(float ms);

public slots:

    void onHeightmapGenerated(const Heightmap& heightmap);

    void onMeshLoaded(const Mesh& mesh);

    void onMeshCreated(const Mesh& mesh);

    void onMeshSaved(bool success);


signals:

	void generateHeightmapDiamond(int iterations, int startAmplitude, float amplitudeModifier);

	void generateHeightmapCircles(int width, int height, int minRadius, int maxRadius, int minAmplitude, int maxAmplitude, int iterations);

	void generateHeightmapPerlin(int size, int octaves, float amplitudeModifier, float frequencyModifier);

	void generateHeightmapFault(int width, int height, int iterations, int startAmplitude, int endAmplitude, int amplitudeChange, HeightmapGenerator::faultFunctions_t function, int transitionLength);

	void createMesh(const Heightmap& heightmap);
	
	void loadMesh(const std::string& filename);

	void saveMesh(const Mesh& mesh, const std::string& filename, const std::string& exportFormatId);

protected:

	void closeEvent(QCloseEvent* event) override;

private:

    Ui::MainWindow* ui;
    QGraphicsScene* m_heightmapScene = nullptr;
	QPixmap m_heightmapPixmap;
	bool m_loadingMesh = false;
	bool m_savingMesh = false;
	bool m_generatingHeightmap = false;

    void lockHeightmap() const;

    void unlockHeightmap() const;

	void updateHeightmapGUI()  const;

    void lockMesh() const;

	void unlockMesh() const;

    void updateMeshGUI() const;

	void updateMaterialGUI() const;

    void resetTransforms()  const;

};
