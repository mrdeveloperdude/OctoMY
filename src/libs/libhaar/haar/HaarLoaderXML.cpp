#include "HaarLoaderXML.hpp"

#include "XMLUtils.hpp"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

/** 
Load Haar cascade from an XML file

<opencv_storage>
<cascade type_id="opencv-cascade-classifier"><stageType>BOOST</stageType>
	<featureType>HAAR</featureType>
	<height>24</height>
	<width>24</width>
	<stageParams>
		<maxWeakCount>211</maxWeakCount></stageParams>
	<featureParams>
		<maxCatCount>0</maxCatCount></featureParams>
	<stageNum>25</stageNum>
	<stages>
		<_>
			<maxWeakCount>9</maxWeakCount>
			<stageThreshold>-5.0425500869750977e+00</stageThreshold>
			<weakClassifiers>
				<_>
					<internalNodes>0 -1 0 -3.1511999666690826e-02</internalNodes>
					<leafValues>2.0875380039215088e+00 -2.2172100543975830e+00</leafValues>
				</_>
				<_>
					<internalNodes>0 -1 1 1.2396000325679779e-02</internalNodes>
					<leafValues>-1.8633940219879150e+00 1.3272049427032471e+00</leafValues>
				</_>
				<_>
					<internalNodes>0 -1 2912 -2.2173000499606133e-02</internalNodes>
					<leafValues>4.8600998520851135e-01 3.6160000599920750e-03</leafValues>
				</_>
			</weakClassifiers>
		</_>
	</stages>
<!-- ... for however many stages there are  -->
		
	<features>
		<_>
			<rects>
				<_>6 4 12 9 -1.</_>
				<_>6 7 12 3 3.</_>
			</rects>
		</_>
		<_>
			<rects>
				<_>6 4 12 7 -1.</_>
				<_>10 4 4 7 3.</_>
			</rects>
		</_>
<!-- ... for however many features there are  -->

	</features>
</cascade>
</opencv_storage>
*/

static const QRegularExpression whitespace("\\s+");

QSharedPointer<HaarCascade> HaarLoaderXML::load(const QString &source) {
	QFile file(source);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "Failed to open XML file:" << source;
		return nullptr;
	}
	
	QDomDocument doc;
	if (!doc.setContent(&file)) {
		qWarning() << "Failed to parse XML file:" << source;
		file.close();
		return nullptr;
	}
	file.close();
	if(mDebug){
		qDebug()<<"Parsed data from '" << source << "'";
	}
	QSharedPointer<HaarCascade> cascade = QSharedPointer<HaarCascade>::create();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "opencv_storage") {
		qWarning() << "Unexpected root element:" << root.tagName();
		return nullptr;
	}
	
	QDomElement cascadeElement = root.firstChildElement("cascade");
	if (cascadeElement.isNull()) {
		qWarning() << "Missing <cascade> element in XML file.";
		return nullptr;
	}
	// Check feature type
	const auto featureType = cascadeElement.firstChildElement("featureType").text().trimmed();
	if (featureType!="HAAR") {
		qWarning() << "Invalid or featureType in XML file.";
		return nullptr;
	}
	// Parse window size
	bool ok{false};
	int windowWidth = cascadeElement.firstChildElement("width").text().trimmed().toInt(&ok);
	if (!ok) {
		qWarning() << "Invalid or missing <width> in XML file.";
		return nullptr;
	}
	int windowHeight = cascadeElement.firstChildElement("height").text().trimmed().toInt(&ok);
	if (!ok) {
		qWarning() << "Invalid or missing <height> in XML file.";
		return nullptr;
	}
	cascade->windowSize = QSize(windowWidth, windowHeight);
	cascade->filename = source;
	cascade->name = QFileInfo(source).completeBaseName();
	
	// Parse features
	QDomElement featuresElement = cascadeElement.firstChildElement("features");
	auto featureNodes = directChildrenByTagName(featuresElement, "_");
	cascade->features.resize(featureNodes.count());
	for (int i = 0; i < featureNodes.count(); ++i) {
		HaarFeature &feature = cascade->features[i];
		QDomElement featureElement = featureNodes.at(i).toElement();
		QDomElement rectsElement = featureElement.firstChildElement("rects");
		auto rectNodes = directChildrenByTagName(rectsElement, "_");
		feature.rects.resize(rectNodes.count());
		for (int j = 0; j < rectNodes.count(); ++j) {
			HaarRect &r = feature.rects[j];
			auto rectElement = rectNodes.at(j).toElement();
			auto rectParts = rectElement.text().trimmed().split(whitespace);
			if (rectParts.size() != 5) {
				qWarning() << "Invalid rect element in feature" << i;
				continue;
			}
			const auto rect_x = rectParts[0].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid rect x '" << rectParts[0] << "' in feature" << i;
				continue;
			}
			const auto rect_y = rectParts[1].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid rect y '" << rectParts[1] << "' in feature" << i;
				continue;
			}
			const auto rect_w = rectParts[2].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid rect w '" << rectParts[2] << "' in feature" << i;
				continue;
			}
			const auto rect_h = rectParts[3].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid rect h '" << rectParts[3] << "' in feature" << i;
				continue;
			}
			r.region.setRect(rect_x, rect_y, rect_w, rect_h);
			r.weight = rectParts[4].toDouble(&ok);
			if (!ok) {
				qWarning() << "Invalid weight in rect element of feature" << i;
				qDebug() << "Invalid weight in rect element of feature" << i;
				continue;
			}
		}
	}
	
	// Parse stages
	QDomElement stagesElement = cascadeElement.firstChildElement("stages");
	auto stageNodes = directChildrenByTagName(stagesElement, "_");
	cascade->stages.resize(stageNodes.count());
	for (int i = 0; i < stageNodes.count(); ++i) {
		HaarStage &stage = cascade->stages[i];
		QDomElement stageElement = stageNodes.at(i).toElement();
		
		// Parse stage threshold
		double stageThreshold = stageElement.firstChildElement("stageThreshold").text().trimmed().toDouble(&ok);
		if (!ok) {
			qWarning() << "Invalid stageThreshold for stage" << i;
			return nullptr;
		}
		stage.stageThreshold = stageThreshold;
		
		// Parse weak classifiers
		QDomElement weakClassifiersElement = stageElement.firstChildElement("weakClassifiers");
		auto classifierNodes = directChildrenByTagName(weakClassifiersElement, "_");
		stage.weakClassifiers.resize(classifierNodes.count());
		for (int j = 0; j < classifierNodes.count(); ++j) {
			HaarWeakClassifier &weakClassifier = stage.weakClassifiers[j];
			QDomElement classifierElement = classifierNodes.at(j).toElement();
			
			// Parse internalNodes
			QStringList internalNodes = classifierElement.firstChildElement("internalNodes").text().trimmed().split(whitespace);
			if (internalNodes.size() != 4) {
				qWarning() << "Invalid internalNodes in classifier" << j << "of stage" << i;
				continue;
			}
			weakClassifier.leftChild = internalNodes[0].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid internalNodes left child index '" << internalNodes[0] << "'in classifier" << j << "of stage" << i;
				continue;
			}
			weakClassifier.rightChild = internalNodes[1].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid internalNodes right child index '" << internalNodes[1] << "' in classifier" << j << "of stage" << i;
				continue;
			}
			weakClassifier.featureIndex = internalNodes[2].toInt(&ok);
			if (!ok) {
				qWarning() << "Invalid internalNodes feature index '" << internalNodes[2] << "' in classifier" << j << "of stage" << i;
				continue;
			}
			weakClassifier.threshold = internalNodes[3].toDouble(&ok);
			if (!ok) {
				qWarning() << "Invalid internalNodes threshold '" << internalNodes[3] << "' in classifier" << j << "of stage" << i;
				continue;
			}
			
			// Parse leafValues
			QStringList leafValues = classifierElement.firstChildElement("leafValues").text().trimmed().split(whitespace);
			if (leafValues.size() != 2) {
				qWarning() << "Invalid leafValues in classifier" << j << "of stage" << i;
				continue;
			}
			weakClassifier.leftVal = leafValues[0].toDouble(&ok);
			if (!ok) {
				qWarning() << "Invalid leftVal '" << leafValues[0] << "' in classifier" << j << "of stage" << i;
				continue;
			}
			weakClassifier.rightVal = leafValues[1].toDouble(&ok);
			if (!ok) {
				qWarning() << "Invalid rightVal '" << leafValues[1] << "' in classifier" << j << "of stage" << i;
				continue;
			}
		}
	}
	
	return cascade;
}

