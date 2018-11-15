#include "QFileDialog"

//�������ָ�
#include "TileSplit/GridSplitStrategy.h"
#include "TileSplit/GeneratorContext.h"

#include "DlgFileSplit.h"

DlgFileSplit::DlgFileSplit(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	CreateConnection();
}

DlgFileSplit::~DlgFileSplit()
{

}

void DlgFileSplit::CreateConnection()
{
	connect(ui.pbtnInputfilename, SIGNAL(clicked()), this, SLOT(OpenInputFile()));
	connect(ui.pbtnOutDir, SIGNAL(clicked()), this, SLOT(OpenOutDir()));

	//ȷ��ȡ��
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(Ok()));
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void DlgFileSplit::OpenInputFile()
{
	//ѡ�������ļ�
	QString fileName = QFileDialog::getOpenFileName(this);
	ui.pbtnInputfilename->setText(fileName);
}

void DlgFileSplit::OpenOutDir()
{
	QString outDir = QFileDialog::getExistingDirectory(this);
	ui.pbtnOutDir->setText(outDir);
}

void DlgFileSplit::Ok()
{
	//����
	QString inputFilename = ui.pbtnInputfilename->text();
	QDir outDir = ui.pbtnOutDir->text();
	unsigned depth = ui.spbxPyraDepth->text().toUInt();

	unsigned boxSample = ui.spbxBoxSample->text().toUInt();
	unsigned levelSample = ui.spbxLevelSample->text().toUInt();
	bool isAll = !ui.cmbxAllLevel->currentIndex();

	//ִ�зֿ�
	DC::Pyra::GeneratorContext context(inputFilename, depth, outDir, isAll);
	DC::Pyra::GridSplitStrategy* gridStrategy = new DC::Pyra::GridSplitStrategy;
	gridStrategy->SetBoxSample(boxSample);
	gridStrategy->SetLevelSample(levelSample);
	context.SetSplitStrategy(gridStrategy);
	context.Generate();

	//�رնԻ���
	accept();
}