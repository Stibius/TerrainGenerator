
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "AssimpIO.h"
#include "Application.h"
#include "Utility.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QScrollBar>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->mainTabWidget);

    m_heightmapScene = new QGraphicsScene(this);
    ui->heightMapGraphicsView->setScene(m_heightmapScene);

	Application::m_materialTextures.insert(Application::m_materialTextures.begin(), Application::MaterialTexture());

    connect(ui->openHeightmapPushButton, &QPushButton::clicked, this, &MainWindow::on_actionOpenHeightmap_triggered);
    connect(ui->saveHeightmapPushButton, &QPushButton::clicked, this, &MainWindow::on_actionSaveHeightmap_triggered);
    connect(ui->openTexturePushButton, &QPushButton::clicked, this, &MainWindow::on_actionOpenTexture_triggered);
    connect(ui->createMeshPushButton, &QPushButton::clicked, this, &MainWindow::on_actionCreateMesh_triggered);
    connect(ui->openMeshPushButton, &QPushButton::clicked, this, &MainWindow::on_actionOpenMesh_triggered);
    connect(ui->saveMeshPushButton, &QPushButton::clicked, this, &MainWindow::on_actionSaveMesh_triggered);
	
    ui->heightMapGraphicsView->addAction(ui->actionOpenHeightmap);
    ui->heightMapGraphicsView->addAction(ui->actionSaveHeightmap);
    ui->myGLWidget->addAction(ui->actionCreateMesh);
    ui->myGLWidget->addAction(ui->actionOpenMesh);
    ui->myGLWidget->addAction(ui->actionSaveMesh);

	const Renderer& renderer = ui->myGLWidget->getRenderer();
	const Mesh& mesh = renderer.getMesh();
	const DirectionalLight& light = renderer.getLight();

	int diamondSize = (1 << ui->diamondIterationsSpinBox->value()) + 1;
	ui->heightMapSizeLabel->setText(QString::number(diamondSize) + " x " + QString::number(diamondSize));
	ui->meshSizeLabel->setText(
		QString::number(mesh.getBoundingBox().m_size.x) +
		" x " +
		QString::number(mesh.getBoundingBox().m_size.y) +
		" x " +
		QString::number(mesh.getBoundingBox().m_size.z));
	ui->fillRadioButton->setChecked(renderer.getMode() == GL_FILL);
	ui->linesRadioButton->setChecked(renderer.getMode() == GL_LINE);
	ui->pointsRadioButton->setChecked(renderer.getMode() == GL_POINT);
	ui->maxYSpinBox->setEnabled(false);
	ui->lightXSpinBox->setValue(light.m_direction.x);
	ui->lightYSpinBox->setValue(light.m_direction.y);
	ui->lightZSpinBox->setValue(light.m_direction.z);
	ui->lightColorPushButton->setStyleSheet("border: 1px solid black; background-color: " + Utility::vecToQColorName(light.m_color));
	ui->shadowsCheckBox->setChecked(renderer.shadowsEnabled());
    ui->poissonSpreadSpinBox->setValue(renderer.getPoissonSpread());
	ui->shadowBiasSpinBox->setValue(renderer.getShadowBias());
	ui->materialHeightBar->setLength(274);
	
	updateMaterialGUI();

    ui->statusBar->showMessage("Ready!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

MyGLWidget& MainWindow::getGLWidget() const
{
	return *ui->myGLWidget;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                               "Terrain Generator",
                                                               tr("Are you sure?\n"),
                                                               QMessageBox::No | QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::lockHeightmap() const
{
    ui->actionOpenHeightmap->setEnabled(false);
    ui->openHeightmapPushButton->setEnabled(false);
    ui->actionSaveHeightmap->setEnabled(false);
    ui->saveHeightmapPushButton->setEnabled(false);
    ui->generateDiamondPushButton->setEnabled(false);
    ui->generateCirclesPushButton->setEnabled(false);
    ui->generatePerlinPushButton->setEnabled(false);
    ui->generateFaultPushButton->setEnabled(false);
    ui->actionCreateMesh->setEnabled(false);
    ui->createMeshPushButton->setEnabled(false);
	ui->heightExpSpinBox->setEnabled(false);
}

void MainWindow::unlockHeightmap() const
{
    ui->actionOpenHeightmap->setEnabled(true);
    ui->openHeightmapPushButton->setEnabled(true);
    ui->generateDiamondPushButton->setEnabled(true);
    ui->generateCirclesPushButton->setEnabled(true);
    ui->generatePerlinPushButton->setEnabled(true);
    ui->generateFaultPushButton->setEnabled(true);

    if (!m_heightmapPixmap.isNull())
    {
        ui->actionSaveHeightmap->setEnabled(true);
        ui->saveHeightmapPushButton->setEnabled(true);
        ui->actionCreateMesh->setEnabled(true);
        ui->createMeshPushButton->setEnabled(true);
		ui->heightExpSpinBox->setEnabled(true);
    }
}

void MainWindow::updateHeightmapGUI() const
{
	m_heightmapScene->clear();
	m_heightmapScene->addPixmap(m_heightmapPixmap);
}

void MainWindow::lockMesh() const
{
    ui->geometryTab->setEnabled(false);
    ui->textureTab1->setEnabled(false);
    ui->lightingTab->setEnabled(false);
    ui->openMeshPushButton->setEnabled(false);
    ui->actionOpenMesh->setEnabled(false);
    ui->saveMeshPushButton->setEnabled(false);
    ui->actionSaveMesh->setEnabled(false);
    ui->createMeshPushButton->setEnabled(false);
    ui->actionCreateMesh->setEnabled(false);
}

void MainWindow::unlockMesh() const
{
    const Mesh& mesh = ui->myGLWidget->getRenderer().getMesh();

	if (!mesh.empty())
	{
		ui->geometryTab->setEnabled(true);
		ui->textureTab1->setEnabled(true);
		ui->lightingTab->setEnabled(true);
		ui->saveMeshPushButton->setEnabled(true);
		ui->actionSaveMesh->setEnabled(true);
	}

	ui->openMeshPushButton->setEnabled(true);
	ui->actionOpenMesh->setEnabled(true);

	if (!m_heightmapPixmap.isNull())
	{
		ui->createMeshPushButton->setEnabled(true);
		ui->actionCreateMesh->setEnabled(true);
	}
}

void MainWindow::updateMeshGUI() const
{
    ui->myGLWidget->updateGL();

	const Mesh& mesh = ui->myGLWidget->getRenderer().getMesh();

    ui->meshSizeLabel->setText(
		QString::number(mesh.getBoundingBox().m_size.x) +
        " x " +
        QString::number(mesh.getBoundingBox().m_size.y) +
        " x " +
        QString::number(mesh.getBoundingBox().m_size.z));
}

void MainWindow::updateMaterialGUI() const
{
	int index = ui->materialHeightBar->getSelectedSection();
	int sectionCount = ui->materialHeightBar->getSectionCount();

	ui->addMatPushButton->setEnabled(ui->materialHeightBar->isSectionDivisible(index));
	ui->removeMatPushButton->setEnabled(sectionCount > 1);
	ui->matUpPushButton->setEnabled(index < sectionCount - 1);
	ui->matDownPushButton->setEnabled(index > 0);

	int materialIndex = ui->materialHeightBar->getSelectedSection();
	const LayeredMaterial& layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	const MaterialLayer& materialLayer = ui->myGLWidget->getRenderer().getMaterial().getMaterialLayer(materialIndex);
	const Material& material = materialLayer.m_material;

	ui->textureLineEdit->setText(Application::m_materialTextures[materialIndex].m_filename);
	ui->ambientRedSpinBox->setValue(material.m_ambient.r * 255);
	ui->ambientGreenSpinBox->setValue(material.m_ambient.g * 255);
	ui->ambientBlueSpinBox->setValue(material.m_ambient.b * 255);
	ui->ambientPushButton->setStyleSheet("border: 1px solid black; background-color: " + Utility::vecToQColorName(material.m_ambient));
	ui->diffuseRedSpinBox->setValue(material.m_diffuse.r * 255);
	ui->diffuseGreenSpinBox->setValue(material.m_diffuse.g * 255);
	ui->diffuseBlueSpinBox->setValue(material.m_diffuse.b * 255);
	ui->diffusePushButton->setStyleSheet("border: 1px solid black; background-color: " + Utility::vecToQColorName(material.m_diffuse));
	ui->specularRedSpinBox->setValue(material.m_specular.r * 255);
	ui->specularGreenSpinBox->setValue(material.m_specular.g * 255);
	ui->specularBlueSpinBox->setValue(material.m_specular.b * 255);
	ui->specularPushButton->setStyleSheet("border: 1px solid black; background-color: " + Utility::vecToQColorName(material.m_specular));
	ui->shininessSpinBox->setValue(material.m_shininess);
	ui->textureCheckBox->setChecked(glIsTexture(layeredMaterial.getTextureID()) && layeredMaterial.hasTexture(materialIndex) && materialLayer.m_textureEnabled);
	ui->textureCheckBox->setEnabled(glIsTexture(layeredMaterial.getTextureID()) && layeredMaterial.hasTexture(materialIndex));
	ui->textureScaleXSpinBox->setValue(material.m_textureScale.x);
	ui->textureScaleYSpinBox->setValue(material.m_textureScale.y);
	ui->textureScaleZSpinBox->setValue(material.m_textureScale.z);
	ui->textureScaleXSpinBox->setEnabled(glIsTexture(layeredMaterial.getTextureID()) && layeredMaterial.hasTexture(materialIndex));
	ui->textureScaleYSpinBox->setEnabled(glIsTexture(layeredMaterial.getTextureID()) && layeredMaterial.hasTexture(materialIndex));
	ui->textureScaleZSpinBox->setEnabled(glIsTexture(layeredMaterial.getTextureID()) && layeredMaterial.hasTexture(materialIndex));
}

void MainWindow::resetTransforms() const
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	Mesh mesh = renderer.getMesh();

	ui->maxYSpinBox->setEnabled(true);
	ui->maxYSpinBox->setValue(mesh.getBoundingBox().m_size.y);

	OrbitPerspectiveCamera camera = renderer.getCamera();
	float distance = sqrt(pow(sqrt(pow(mesh.getBoundingBox().m_size.x, 2.0) + pow(mesh.getBoundingBox().m_size.y, 2.0)), 2.0) + pow(mesh.getBoundingBox().m_size.z, 2.0));
	camera.m_orbitCamera.setRotation(glm::vec2(45.0f, 45.0f));
	camera.m_orbitCamera.setDistance(distance);
	renderer.setCamera(camera);
	Application::m_mouseEventHandler.setZoomSpeed(distance / 100);

    ui->lightXSpinBox->setValue(0.0f);
    ui->lightYSpinBox->setValue(-1.0f);
    ui->lightZSpinBox->setValue(0.0f);
	DirectionalLight light = renderer.getLight();
	light.m_direction = glm::vec4(ui->lightXSpinBox->value(), ui->lightYSpinBox->value(), ui->lightZSpinBox->value(), 0.0f);
	renderer.setLight(light);

    updateMeshGUI();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionOpenHeightmap_triggered()
{
    QString selectedFilter = "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff)";

    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open Height Map",
                                                    QDir::currentPath(),
                                                    "All files (*.*);;"
                                                    "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff);;"
                                                    "bitmap image (*.bmp);; "
                                                    "JPEG (*.jpg;*.jpeg);;"
                                                    "PNG (*.png);;"
                                                    "GIF (*.gif);;"
                                                    "TIF (*.tif;*.tiff)",
                                                    &selectedFilter);

    if (!filename.isEmpty())
    {   
		m_heightmapPixmap = QPixmap(filename);
		Application::m_heightmapOrig = Utility::QPixmapToHeightmap(m_heightmapPixmap);
		Application::m_heightmap = Application::m_heightmapOrig;

        if (m_heightmapPixmap.isNull())
        {
            QMessageBox::information(this, "Terrain Generator", "Error while reading from file\n" + filename);
        }
        else
        {
			ui->heightExpSpinBox->setValue(1.0);
            unlockHeightmap();
			updateHeightmapGUI();
        }
    }
}

void MainWindow::on_actionSaveHeightmap_triggered()
{
    QString selectedFilter = "PNG (*.png)";

    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Save Height Map",
                                                    QDir::currentPath(),
                                                    "All files (*.*);;"
                                                    "bitmap image (*.bmp);; "
                                                    "JPEG (*.jpg;*.jpeg);;"
                                                    "PNG (*.png);;"
                                                    "GIF (*.gif);;"
                                                    "TIF (*.tif;*.tiff)",
                                                    &selectedFilter);


    if (!filename.isEmpty())
    {
        if (!m_heightmapPixmap.save(filename))
        {
            QMessageBox::information(this, "Terrain Generator", "Error while saving to file\n" + filename);
        }
    }
}

void MainWindow::on_diamondIterationsSpinBox_valueChanged(int arg1)
{
    int size = (1 << arg1) + 1;
    ui->heightMapSizeLabel->setText(QString::number(size) + " x " + QString::number(size));
}

void MainWindow::on_generateDiamondPushButton_clicked()
{
	lockHeightmap();

	ui->statusBar->showMessage("Generating heightmap...");

	emit generateHeightmapDiamond(
		ui->diamondIterationsSpinBox->value(),
		ui->diamondAmplitudeSpinBox->value(),
		ui->diamondModifierSpinBox->value());
}

void MainWindow::on_generateCirclesPushButton_clicked()
{
    bool valuesOK = true;

    if (ui->circlesMinAmplitudeSpinBox->value() > ui->circlesMaxAmplitudeSpinBox->value())
    {
        QMessageBox::information(this,
                                 "Terrain Generator",
                                 "Min. amplitude can't be set higher than max. amplitude!");

        valuesOK = false;
    }

    if (ui->circlesMinRadiusSpinBox->value() > ui->circlesMaxRadiusSpinBox->value())
    {
        QMessageBox::information(this,
                                 "Terrain Generator",
                                 "Min. radius can't be set higher than max. radius!");

        valuesOK = false;
    }

    if (!valuesOK) return;

	lockHeightmap();

	ui->statusBar->showMessage("Generating heightmap...");

	emit generateHeightmapCircles(
		ui->circlesWidthSpinBox->value(),
		ui->circlesLengthSpinBox->value(),
		ui->circlesMinRadiusSpinBox->value(),
		ui->circlesMaxRadiusSpinBox->value(),
		ui->circlesMinAmplitudeSpinBox->value(),
		ui->circlesMaxAmplitudeSpinBox->value(),
		ui->circlesIterationsSpinBox->value());
}

void MainWindow::on_generatePerlinPushButton_clicked()
{
	lockHeightmap();

	ui->statusBar->showMessage("Generating heightmap...");

	emit generateHeightmapPerlin(
		ui->perlinSizeSpinBox->value(),
		ui->perlinOctavesSpinBox->value(),
		ui->perlinAmplitudeModifierSpinBox->value(),
		ui->perlinFrequencyModifierSpinBox->value());
}

void MainWindow::on_generateFaultPushButton_clicked()
{
	lockHeightmap();

	ui->statusBar->showMessage("Generating heightmap...");

	emit generateHeightmapFault(
		ui->faultWidthSpinBox->value(),
		ui->faultLengthSpinBox->value(),
		ui->faultIterationsSpinBox->value(),
		ui->faultStartAmplitudeSpinBox->value(),
		ui->faultEndAmplitudeSpinBox->value(),
		ui->faultAmplitudeChangeSpinBox->value(),
		HeightmapGenerator::faultFunctions_t(ui->faultFunctionComboBox->currentIndex()),
		ui->faultTransitionLengthSpinBox->value());
}

void MainWindow::on_heightExpSpinBox_valueChanged(double arg1)
{
	for (int row = 0; row < Application::m_heightmapOrig.getHeight(); ++row)
	{
		for (int col = 0; col < Application::m_heightmapOrig.getWidth(); ++col)
		{
			Application::m_heightmap.at(row, col) = pow(Application::m_heightmapOrig.at(row, col), arg1);
		}
	}

	m_heightmapPixmap = Utility::heightmapToQPixmap(Application::m_heightmap);

	updateHeightmapGUI();
}

void MainWindow::on_actionCreateMesh_triggered()
{
	emit createMesh(Application::m_heightmap);
    
    lockMesh();

    ui->statusBar->showMessage("Creating mesh...");
}

void MainWindow::on_actionOpenMesh_triggered()
{
	std::vector<std::string> extensions = AssimpIO::getImportExtensions();
	QString filters = "All files (*.*);;";
	for (const std::string& extension : extensions)
	{
		filters += "(" + QString::fromStdString(extension) + ");;";
	}
    QString selectedFilter = "All files (*.*)";

    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open Terrain Model",
                                                    QDir::currentPath(),
                                                    filters,
                                                    &selectedFilter);

    if (!filename.isEmpty())
    {
		lockMesh();

		ui->statusBar->showMessage("Loading mesh...");

		emit loadMesh(filename.toStdString());
    }
}

void MainWindow::on_actionSaveMesh_triggered()
{
	std::vector<AssimpIO::ExportFormat> formats = AssimpIO::getExportFormats();
	QString filters = "All files (*.*);;";
	for (const AssimpIO::ExportFormat& format : formats)
	{
		filters += QString::fromStdString(format.description) 
			+ " (*." 
			+ QString::fromStdString(format.extension)
			+ ");;";
	}

	QFileDialog dialog(this, "Save mesh", QDir::currentPath(), filters);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if (dialog.exec() == QDialog::Accepted)
	{
		QString selectedFilter = dialog.selectedNameFilter();
		QString filename = dialog.selectedFiles()[0];
		QStringList filterList = filters.split(";;");
		int filterIndex;
		for (filterIndex = 0; filterIndex < filterList.count(); filterIndex++)
		{
			if (filterList[filterIndex] == selectedFilter)
			{
				break;
			}
		}

		AssimpIO::ExportFormat exportFormat;
		if (filterIndex == 0)
		{
			int start = filename.lastIndexOf('.') + 1;
			QStringRef extension(&filename, start, filename.size() - start);
			for (const AssimpIO::ExportFormat& format : formats)
			{
				if (QString::fromStdString(format.extension) == extension)
				{
					exportFormat = format;
					break;
				}
			}
		}
		else
		{
			exportFormat = formats[filterIndex - 1];
		}

		lockMesh();

		ui->statusBar->showMessage("Saving mesh...");

		const Mesh& mesh = ui->myGLWidget->getRenderer().getMesh();

		emit saveMesh(mesh, filename.toStdString(), exportFormat.id);		
	}
}

void MainWindow::on_ambientPushButton_clicked()
{
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_ambient);
	QColor newColor = QColorDialog::getColor(color, this, QString());
	if (newColor.isValid())
	{
		ui->ambientRedSpinBox->setValue(newColor.red());
		ui->ambientGreenSpinBox->setValue(newColor.green());
		ui->ambientBlueSpinBox->setValue(newColor.blue());
		ui->ambientPushButton->setStyleSheet("border: 1px solid black; background-color: " + newColor.name());
		materialLayer.m_material.m_ambient = Utility::QColorToVec(newColor);
		layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
		ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);
		ui->myGLWidget->updateGL();
	}
}

void MainWindow::on_diffusePushButton_clicked()
{
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_diffuse);
	QColor newColor = QColorDialog::getColor(color, this, QString());
	if (newColor.isValid())
	{
		ui->diffuseRedSpinBox->setValue(newColor.red());
		ui->diffuseGreenSpinBox->setValue(newColor.green());
		ui->diffuseBlueSpinBox->setValue(newColor.blue());
		ui->diffusePushButton->setStyleSheet("border: 1px solid black; background-color: " + newColor.name());
		materialLayer.m_material.m_diffuse = Utility::QColorToVec(newColor);
		layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
		ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);
		ui->myGLWidget->updateGL();
	}
}

void MainWindow::on_specularPushButton_clicked()
{
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_specular);
	QColor newColor = QColorDialog::getColor(color, this, QString());
	if (newColor.isValid())
	{
		ui->specularRedSpinBox->setValue(newColor.red());
		ui->specularGreenSpinBox->setValue(newColor.green());
		ui->specularBlueSpinBox->setValue(newColor.blue());
		ui->specularPushButton->setStyleSheet("border: 1px solid black; background-color: " + newColor.name());
		materialLayer.m_material.m_specular = Utility::QColorToVec(newColor);
		layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
		ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);
		ui->myGLWidget->updateGL();
	}
}

void MainWindow::onHeightmapGenerated(const Heightmap& heightmap)
{
	Application::m_heightmapOrig = heightmap;
	Application::m_heightmap = Application::m_heightmapOrig;
	m_heightmapPixmap = Utility::heightmapToQPixmap(Application::m_heightmap);

    if (!Application::m_heightmap.isEmpty())
    {
        QScrollBar* vScrollBar = ui->heightMapGraphicsView->verticalScrollBar();
        QScrollBar* hScrollBar = ui->heightMapGraphicsView->horizontalScrollBar();
        vScrollBar->setValue(0);
        hScrollBar->setValue(0);
		ui->heightExpSpinBox->setValue(1.0);
    }
    else
    {
        QMessageBox::information(this,
                                 "Terrain Generator",
                                 "Not enough memory to create heightmap of the specified size!");
    }

    unlockHeightmap();
	updateHeightmapGUI();

    if (Application::m_concurrencyHandler.isLoadingMesh())
    {
        ui->statusBar->showMessage("Loading mesh...");
    }
    else if (Application::m_concurrencyHandler.isSavingMesh())
    {
        ui->statusBar->showMessage("Saving mesh...");
    }
    else
    {
        ui->statusBar->showMessage("Ready!");
    }
}

void MainWindow::onMeshLoaded(const Mesh& mesh)
{
    if (mesh.empty())
    {
        QMessageBox::information(this,
                                 "Terrain Generator",
                                 "Error while reading from file\n");
    }
    else
    {
		ui->myGLWidget->getRenderer().setMesh(mesh);

        resetTransforms();
    }

	unlockMesh();

    if (Application::m_concurrencyHandler.isGeneratingHeightmap())
    {
        ui->statusBar->showMessage("Generating heightmap...");
    }
    else
    {
        ui->statusBar->showMessage("Ready!");
    }
}

void MainWindow::onMeshCreated(const Mesh& mesh)
{
    if (mesh.empty())
    {
        QMessageBox::information(this,
                                 "Terrain Generator",
                                 "Error while creating mesh from heightmap");
    }
    else
    {
		ui->myGLWidget->getRenderer().setMesh(mesh);

        resetTransforms();
    }

	unlockMesh();

    if (Application::m_concurrencyHandler.isGeneratingHeightmap())
    {
        ui->statusBar->showMessage("Generating heightmap...");
    }
    else
    {
        ui->statusBar->showMessage("Ready!");
    }
}

void MainWindow::onMeshSaved(bool success)
{
    if (success == false)
    {
        QMessageBox::information(this, "Terrain Generator",
                                       "Error while saving to file\n");
    }

	unlockMesh();

    if (Application::m_concurrencyHandler.isGeneratingHeightmap())
    {
        ui->statusBar->showMessage("Generating heightmap...");
    }
    else
    {
        ui->statusBar->showMessage("Ready!");
    }
}

void MainWindow::on_maxYSpinBox_valueChanged(double arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	Mesh mesh = renderer.getMesh();

	mesh.setHeight(arg1);

	renderer.setMesh(mesh);

	updateMeshGUI();
}

void MainWindow::on_textureCheckBox_clicked(bool checked)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_textureEnabled = checked;
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	QColor color = Utility::vecToQColor(materialLayer.m_material.m_diffuse);
	if (checked)
	{
		ui->materialHeightBar->setSectionColor(materialIndex, Utility::getAverageColor(Application::m_materialTextures[materialIndex].m_image));
	}
	else
	{
		ui->materialHeightBar->setSectionColor(materialIndex, color);
	}

    updateMeshGUI();
}

void MainWindow::on_textureScaleXSpinBox_valueChanged(double arg1)
{
	int index = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(index);
	materialLayer.m_material.m_textureScale.x = arg1;
	layeredMaterial.setMaterialLayer(index, materialLayer);
	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMeshGUI();
}

void MainWindow::on_textureScaleYSpinBox_valueChanged(double arg1)
{
	int index = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(index);
	materialLayer.m_material.m_textureScale.y = arg1;
	layeredMaterial.setMaterialLayer(index, materialLayer);
	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMeshGUI();
}

void MainWindow::on_textureScaleZSpinBox_valueChanged(double arg1)
{
	int index = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(index);
	materialLayer.m_material.m_textureScale.z = arg1;
	layeredMaterial.setMaterialLayer(index, materialLayer);
	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMeshGUI();
}

void MainWindow::on_fillRadioButton_clicked(bool checked)
{
    if (checked == true)
    {
        ui->myGLWidget->getRenderer().setMode(GL_FILL);
        updateMeshGUI();
    }
}

void MainWindow::on_linesRadioButton_clicked(bool checked)
{
    if (checked == true)
    {
        ui->myGLWidget->getRenderer().setMode(GL_LINE);
        updateMeshGUI();
    }
}

void MainWindow::on_pointsRadioButton_clicked(bool checked)
{
    if (checked == true)
    {
        ui->myGLWidget->getRenderer().setMode(GL_POINT);
        updateMeshGUI();
    }
}

void MainWindow::on_actionOpenTexture_triggered()
{
    QString selectedFilter = "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff)";

    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open Texture",
                                                    QDir::currentPath(),
                                                    "All files (*.*);;"
                                                    "Image Files (*.bmp;*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff);;"
                                                    "bitmap image (*.bmp);; "
                                                    "JPEG (*.jpg;*.jpeg);;"
                                                    "PNG (*.png);;"
                                                    "GIF (*.gif);;"
                                                    "TIF (*.tif;*.tiff)",
                                                    &selectedFilter);

    if (!filename.isEmpty())
    {
		QImage newTextureImage = QImage(filename);

        if (newTextureImage.isNull())
        {
            QMessageBox::information(this,
                                     "Terrain Generator",
                                     "Error while reading from file\n" + filename);

			ui->textureLineEdit->setText("");

            return;
        }

		int materialIndex = ui->materialHeightBar->getSelectedSection();
		Application::m_materialTextures[materialIndex].m_image = newTextureImage;
		Application::m_materialTextures[materialIndex].m_filename = filename;
		ui->materialHeightBar->setSectionColor(materialIndex, Utility::getAverageColor(newTextureImage));
		QSize largestSize = QSize(0, 0);
		for (const Application::MaterialTexture& texture : Application::m_materialTextures)
		{
			const QImage& textureImage = texture.m_image;
			QSize size = textureImage.size();
			if (size.width() * size.height() > largestSize.width() * largestSize.height())
			{
				largestSize = size;
			}
		}
		std::vector<QImage> images;
		images.reserve(Application::m_materialTextures.size());
		std::vector<const void*> pixels;
		pixels.reserve(Application::m_materialTextures.size());
		for (Application::MaterialTexture& texture : Application::m_materialTextures)
		{
			QImage& textureImage = texture.m_image;
			if (!textureImage.isNull())
			{
				textureImage = textureImage.scaled(largestSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			}
			images.push_back(QGLWidget::convertToGLFormat(textureImage));
			pixels.push_back(images.back().bits());
		}

		LayeredMaterial material = ui->myGLWidget->getRenderer().getMaterial();

		material.init(
			material.getMaterialLayers(),
			largestSize.width(),
			largestSize.height(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pixels);

		ui->myGLWidget->getRenderer().setMaterial(material);

		ui->textureLineEdit->setText(filename);
		ui->textureLineEdit->setCursorPosition(0);
		
		updateMaterialGUI();
		
		ui->myGLWidget->updateGL();
    }
}

void MainWindow::on_lightXSpinBox_valueChanged(double arg1)
{
	double x = ui->lightXSpinBox->value();
	double y = ui->lightYSpinBox->value();
	double z = ui->lightZSpinBox->value();

	Renderer& renderer = ui->myGLWidget->getRenderer();
	DirectionalLight light = renderer.getLight();

	light.m_direction = glm::normalize(glm::vec4(x, y, z, 0.0f));
	renderer.setLight(light);
    ui->myGLWidget->updateGL();
}

void MainWindow::on_lightYSpinBox_valueChanged(double arg1)
{
	double x = ui->lightXSpinBox->value();
	double y = ui->lightYSpinBox->value();
	double z = ui->lightZSpinBox->value();

	Renderer& renderer = ui->myGLWidget->getRenderer();
	DirectionalLight light = renderer.getLight();

	light.m_direction = glm::normalize(glm::vec4(x, y, z, 0.0f));
	renderer.setLight(light);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_lightZSpinBox_valueChanged(double arg1)
{
	double x = ui->lightXSpinBox->value();
	double y = ui->lightYSpinBox->value();
	double z = ui->lightZSpinBox->value();

	Renderer& renderer = ui->myGLWidget->getRenderer();
	DirectionalLight light = renderer.getLight();

	light.m_direction = glm::normalize(glm::vec4(x, y, z, 0.0f));
	renderer.setLight(light);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_lightColorPushButton_clicked()
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	DirectionalLight light = renderer.getLight();

	QColor color = QColorDialog::getColor(Utility::vecToQColor(light.m_color), this, QString());
    if (color.isValid())
    {
        ui->lightColorPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
		light.m_color = Utility::QColorToVec(color);
		renderer.setLight(light);
		ui->myGLWidget->updateGL();
    }
}

void MainWindow::on_addMatPushButton_clicked()
{
	int index = ui->materialHeightBar->getSelectedSection();
	if (ui->materialHeightBar->addSection(index))
	{
		LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
		std::vector<MaterialLayer> materialLayers = layeredMaterial.getMaterialLayers();
		for (auto it = materialLayers.begin(); it != materialLayers.end(); ++it)
		{
			if (it == materialLayers.begin() + index)
			{
				MaterialLayer materialLayer = MaterialLayer(Material(), ui->materialHeightBar->getSectionUpperBound(index));
				it = materialLayers.insert(it, materialLayer);
				ui->materialHeightBar->setSectionColor(index, Utility::vecToQColor(materialLayer.m_material.m_diffuse));
			}
			else
			{
				it->m_maxY = ui->materialHeightBar->getSectionUpperBound(it - materialLayers.begin());
			}
		}

		Application::m_materialTextures.insert(Application::m_materialTextures.begin() + index, Application::MaterialTexture());
		QSize textureSize = QSize(0, 0);
		std::vector<QImage> images;
		images.reserve(Application::m_materialTextures.size());
		std::vector<const void*> pixels;
		pixels.reserve(Application::m_materialTextures.size());
		for (Application::MaterialTexture& texture : Application::m_materialTextures)
		{
			QImage& textureImage = texture.m_image;
			images.push_back(QGLWidget::convertToGLFormat(textureImage));
			pixels.push_back(images.back().bits());
			if (textureImage.width() != 0 && textureImage.height() != 0)
			{
				textureSize = textureImage.size();
			}
		}

		if (textureSize.width() != 0 && textureSize.height() != 0)
		{
			layeredMaterial.init(
				materialLayers,
				textureSize.width(),
				textureSize.height(),
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				pixels);
		}
		else
		{
			layeredMaterial.init(materialLayers);
		}

		ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

		updateMaterialGUI();

		ui->myGLWidget->updateGL();
	}
}

void MainWindow::on_removeMatPushButton_clicked()
{
	int index = ui->materialHeightBar->getSelectedSection();

	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	std::vector<MaterialLayer> materialLayers = layeredMaterial.getMaterialLayers();

	ui->materialHeightBar->removeSection(index);

	materialLayers.erase(materialLayers.begin() + index);
	if (index > 0)
	{
		materialLayers[index - 1].m_maxY = ui->materialHeightBar->getSectionUpperBound(index - 1);
	}

	Application::m_materialTextures.erase(Application::m_materialTextures.begin() + index);

	QSize textureSize = QSize(0, 0);
	std::vector<QImage> images;
	images.reserve(Application::m_materialTextures.size());
	std::vector<const void*> pixels;
	pixels.reserve(Application::m_materialTextures.size());
	for (Application::MaterialTexture& texture : Application::m_materialTextures)
	{
		QImage& textureImage = texture.m_image;
		images.push_back(QGLWidget::convertToGLFormat(textureImage));
		pixels.push_back(images.back().bits());
		if (textureImage.width() != 0 && textureImage.height() != 0)
		{
			textureSize = textureImage.size();
		}
	}

	if (textureSize.width() != 0 && textureSize.height() != 0)
	{
		layeredMaterial.init(
			materialLayers,
			textureSize.width(),
			textureSize.height(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pixels);
	}
	else
	{
		layeredMaterial.init(materialLayers);
	}


	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMaterialGUI();

	ui->myGLWidget->updateGL();
}

void MainWindow::on_matUpPushButton_clicked()
{
	int index = ui->materialHeightBar->getSelectedSection();

	ui->materialHeightBar->moveSectionUp(index);
	std::swap(Application::m_materialTextures[index], Application::m_materialTextures[index + 1]);

	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	std::vector<MaterialLayer> materialLayers = layeredMaterial.getMaterialLayers();
	
	MaterialLayer& lowerLayer = materialLayers[index];
	MaterialLayer& upperLayer = materialLayers[index + 1];
	float lowerMaxY = lowerLayer.m_maxY;
	float upperMaxY = upperLayer.m_maxY;

	lowerLayer.m_maxY = upperMaxY;
	if (index - 1 >= 0)
	{
		upperLayer.m_maxY = materialLayers[index - 1].m_maxY + (upperMaxY - lowerMaxY);
	}
	else
	{
		upperLayer.m_maxY = upperMaxY - lowerMaxY;
	}

	std::swap(lowerLayer, upperLayer);

	QSize textureSize = QSize(0, 0);
	std::vector<QImage> images;
	images.reserve(Application::m_materialTextures.size());
	std::vector<const void*> pixels;
	pixels.reserve(Application::m_materialTextures.size());
	for (Application::MaterialTexture& texture : Application::m_materialTextures)
	{
		QImage& textureImage = texture.m_image;
		images.push_back(QGLWidget::convertToGLFormat(textureImage));
		pixels.push_back(images.back().bits());
		if (textureImage.width() != 0 && textureImage.height() != 0)
		{
			textureSize = textureImage.size();
		}
	}

	if (textureSize.width() != 0 && textureSize.height() != 0)
	{
		layeredMaterial.init(
			materialLayers,
			textureSize.width(),
			textureSize.height(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pixels);
	}
	else
	{
		layeredMaterial.init(materialLayers);
	}

	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMaterialGUI();

	ui->myGLWidget->updateGL();
}

void MainWindow::on_matDownPushButton_clicked()
{
	int index = ui->materialHeightBar->getSelectedSection();

	ui->materialHeightBar->moveSectionDown(index);
	std::swap(Application::m_materialTextures[index], Application::m_materialTextures[index - 1]);

	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	std::vector<MaterialLayer> materialLayers = layeredMaterial.getMaterialLayers();

	MaterialLayer& lowerLayer = materialLayers[index - 1];
	MaterialLayer& upperLayer = materialLayers[index];
	float lowerMaxY = lowerLayer.m_maxY;
	float upperMaxY = upperLayer.m_maxY;

	lowerLayer.m_maxY = upperMaxY;
	if (index - 2 >= 0)
	{
		upperLayer.m_maxY = materialLayers[index - 2].m_maxY + (upperMaxY - lowerMaxY);
	}
	else
	{
		upperLayer.m_maxY = upperMaxY - lowerMaxY;
	}

	std::swap(lowerLayer, upperLayer);

	QSize textureSize = QSize(0, 0);
	std::vector<QImage> images;
	images.reserve(Application::m_materialTextures.size());
	std::vector<const void*> pixels;
	pixels.reserve(Application::m_materialTextures.size());
	for (Application::MaterialTexture& texture : Application::m_materialTextures)
	{
		QImage& textureImage = texture.m_image;
		images.push_back(QGLWidget::convertToGLFormat(textureImage));
		pixels.push_back(images.back().bits());
		if (textureImage.width() != 0 && textureImage.height() != 0)
		{
			textureSize = textureImage.size();
		}
	}

	if (textureSize.width() != 0 && textureSize.height() != 0)
	{
		layeredMaterial.init(
			materialLayers,
			textureSize.width(),
			textureSize.height(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pixels);
	}
	else
	{
		layeredMaterial.init(materialLayers);
	}

	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMaterialGUI();

	ui->myGLWidget->updateGL();
}

void MainWindow::on_materialHeightBar_selectedSectionChanged(int index)
{
	updateMaterialGUI();
}

void MainWindow::on_materialHeightBar_sectionHeightsChanged()
{
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	std::vector<MaterialLayer> materialLayers = layeredMaterial.getMaterialLayers();
	for (auto it = materialLayers.begin(); it != materialLayers.end(); ++it)
	{
		it->m_maxY = ui->materialHeightBar->getSectionUpperBound(it - materialLayers.begin());
	}

	QSize textureSize = QSize(0, 0);
	std::vector<QImage> images;
	images.reserve(Application::m_materialTextures.size());
	std::vector<const void*> pixels;
	pixels.reserve(Application::m_materialTextures.size());
	for (Application::MaterialTexture& texture : Application::m_materialTextures)
	{
		QImage& textureImage = texture.m_image;
		images.push_back(QGLWidget::convertToGLFormat(textureImage));
		pixels.push_back(images.back().bits());
		if (textureImage.width() != 0 && textureImage.height() != 0)
		{
			textureSize = textureImage.size();
		}
	}

	if (textureSize.width() != 0 && textureSize.height() != 0)
	{
		layeredMaterial.init(
			materialLayers,
			textureSize.width(),
			textureSize.height(),
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			pixels);
	}
	else
	{
		layeredMaterial.init(materialLayers);
	}

	ui->myGLWidget->getRenderer().setMaterial(layeredMaterial);

	updateMaterialGUI();

	ui->myGLWidget->updateGL();
}

void MainWindow::on_ambientRedSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_ambient.r = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_ambient);
	ui->ambientPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_ambientGreenSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_ambient.g = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_ambient);
	ui->ambientPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_ambientBlueSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_ambient.b = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_ambient);
	ui->ambientPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_diffuseRedSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_diffuse.r = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_diffuse);
	ui->diffusePushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	if (!ui->textureCheckBox->isChecked())
	{
		ui->materialHeightBar->setSectionColor(materialIndex, color);
	}
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_diffuseGreenSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_diffuse.g = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_diffuse);
	ui->diffusePushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	if (!ui->textureCheckBox->isChecked())
	{
		ui->materialHeightBar->setSectionColor(materialIndex, color);
	}
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_diffuseBlueSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_diffuse.b = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_diffuse);
	ui->diffusePushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	if (!ui->textureCheckBox->isChecked())
	{
		ui->materialHeightBar->setSectionColor(materialIndex, color);
	}
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_specularRedSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_specular.r = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_specular);
	ui->specularPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_specularGreenSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_specular.g = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_specular);
	ui->specularPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_specularBlueSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_specular.b = arg1 / 255.0f;
	QColor color = Utility::vecToQColor(materialLayer.m_material.m_specular);
	ui->specularPushButton->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_shininessSpinBox_valueChanged(int arg1)
{
	Renderer& renderer = ui->myGLWidget->getRenderer();
	int materialIndex = ui->materialHeightBar->getSelectedSection();
	LayeredMaterial layeredMaterial = ui->myGLWidget->getRenderer().getMaterial();
	MaterialLayer materialLayer = layeredMaterial.getMaterialLayer(materialIndex);
	materialLayer.m_material.m_shininess = arg1;
	layeredMaterial.setMaterialLayer(materialIndex, materialLayer);
	renderer.setMaterial(layeredMaterial);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_shadowsCheckBox_toggled(bool checked)
{
    ui->myGLWidget->getRenderer().enableShadows(checked);
    ui->myGLWidget->updateGL();
}

void MainWindow::on_poissonSpreadSpinBox_valueChanged(int arg1)
{
    ui->myGLWidget->getRenderer().setPoissonSpread(arg1);
    ui->myGLWidget->updateGL();
}

void MainWindow::on_shadowBiasSpinBox_valueChanged(double arg1)
{
	ui->myGLWidget->getRenderer().setShadowBias(arg1);
	ui->myGLWidget->updateGL();
}

void MainWindow::on_myGLWidget_renderingFinished(float ms)
{
	ui->renderTimeLabel->setText(QString::number(ms) + " ms");
}
