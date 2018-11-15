#pragma once

#include "QString"
#include "QStringList"
#include "QFileInfo"
#include "QDir"

struct FileInfo
{
	//! �ļ�����ʵ��
	//! changshu_L0_X0_Y0.pfa

	FileInfo(const QString& inFilename)
		: levelID("-1")
		, xID("-1")
		, yID("-1")
		, zID("-1")
		, suffix(".pdb")
	{
		//���ݴ����ļ�������ʼ����������ֵ
		PraseFile(inFilename);
	}

	QStringList GenNameByIndex(int level, QString index)
	{
		QStringList list;

		if (fAlg == QuadTree)
		{
			//���к�
			unsigned column = index.split("_").at(0).toInt() * 2;
			unsigned row = index.split("_").at(1).toInt() * 2;

			//! ��·�� + ��ҳ�ļ���ʽ�� + ��׺��
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column).arg(row) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column + 1).arg(row) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column).arg(row + 1) + suffix);
			list.push_back(absoultFilePath + "\\" + QString("%1_L%2_X%3_Y%4").arg(rootFileName).arg(level).arg(column + 1).arg(row + 1) + suffix);

			/*list << "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y0.pfa"
			<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y0.pfa"
			<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X0_Y1.pfa"
			<< "F:\\testbig\\process\\changshu_pfa\\changshu_pagefile\\changshu_L1_X1_Y1.pfa";*/
		}
		else if (fAlg == Octree)
		{
			//���к�
			QStringList iList = index.split("_");
			Q_ASSERT(iList.size() == 3);
			unsigned column = iList[0].toInt() * 2;
			unsigned row = iList[1].toInt() * 2;
			unsigned layer = iList[2].toInt() * 2;

			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row + 1));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row + 1));

			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column).arg(row + 1));
			list.push_back(QString("%1_L%2_X%3_Y%4_Z%5").arg(level).arg(column + 1).arg(row + 1));
		}

		return list;
	}

	void PraseFile(const QString& inFilename)
	{
		//�ж��ļ�����basename���Ƿ����_�ַ�
		QFileInfo fInfo(inFilename);
		QString baseName = fInfo.baseName();
		absoultFilePath = QDir::toNativeSeparators(fInfo.absolutePath());
		rootFileName = baseName.split("_").at(0);

		bool hasUnderline = baseName.contains("_");
		int underLineNum = baseName.count("_");

		//! ����issubfile
		isSubfile = hasUnderline;

		//�����ļ��ָ��㷨������ȷ�����ļ�����
		if (underLineNum == 3)
		{
			fAlg = QuadTree;

			//! ��ʼ��IDֵ��list
			//! �ָ�basename
			QStringList bnList = baseName.split("_");
			levelID = bnList[1].remove(0, 1);
			xID = bnList[2].remove(0, 1);
			yID = bnList[3].remove(0, 1);

			//���subfile,���ݵ�ǰ��id������filename
			QString index = QString("%1_%2").arg(xID).arg(yID);
			subFileList = GenNameByIndex(levelID.toInt() + 1, index);
		}
		else if (underLineNum == 4)
		{
			fAlg = Octree;
			//! ��ʼ��IDֵ��list
			//! �ָ�basename
			QStringList bnList = baseName.split("_");
			levelID = bnList[1].remove(0, 1);
			xID = bnList[2].remove(0, 1);
			yID = bnList[3].remove(0, 1);
			zID = bnList[4].remove(0, 1);

		}
		else
		{
			fAlg = Unknown;
		}

	}

	enum FileAlgorithm
	{
		QuadTree = 0,
		Octree = 1,
		Unknown = 2
	};

	//�ļ����ֲ���
	QString absoultFilePath;
	QString rootFileName;
	QString suffix;

	QString levelID;
	QString xID;
	QString yID;
	QString zID;
	QStringList subFileList;

	FileAlgorithm fAlg;

	bool isSubfile;
};
