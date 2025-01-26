#include "HaarSaverXML.hpp"
#include <QDomDocument>
#include <QFile>
#include <QDebug>

// Save Haar cascade to an XML file
void HaarSaverXML::save(const HaarCascade &cascade, const QString &destination) {
	QDomDocument doc;
	QDomElement root = doc.createElement("opencv_storage");
	doc.appendChild(root);
	
	// Add <cascade> element
	QDomElement cascadeElement = doc.createElement("cascade");
	root.appendChild(cascadeElement);
	
	// Add <stageType> and <featureType>
	cascadeElement.appendChild(doc.createElement("stageType")).appendChild(doc.createTextNode("BOOST"));
	cascadeElement.appendChild(doc.createElement("featureType")).appendChild(doc.createTextNode("HAAR"));
	
	// Add window size (height and width)
	cascadeElement.appendChild(doc.createElement("height")).appendChild(doc.createTextNode(QString::number(cascade.windowSize.height())));
	cascadeElement.appendChild(doc.createElement("width")).appendChild(doc.createTextNode(QString::number(cascade.windowSize.width())));
	
	// Add features
	QDomElement featuresElement = doc.createElement("features");
	cascadeElement.appendChild(featuresElement);
	for (const HaarFeature &feature : cascade.features) {
		QDomElement featureElement = doc.createElement("_");
		featuresElement.appendChild(featureElement);
		
		// Add rects
		QDomElement rectsElement = doc.createElement("rects");
		featureElement.appendChild(rectsElement);
		for (const HaarRect &rect : feature.rects) {
			QDomElement rectElement = doc.createElement("_");
			QString rectText = QString("%1 %2 %3 %4 %5")
								   .arg(rect.region.x())
								   .arg(rect.region.y())
								   .arg(rect.region.width())
								   .arg(rect.region.height())
								   .arg(rect.weight);
			rectElement.appendChild(doc.createTextNode(rectText));
			rectsElement.appendChild(rectElement);
		}
	}
	
	// Add stages
	QDomElement stagesElement = doc.createElement("stages");
	cascadeElement.appendChild(stagesElement);
	for (const HaarStage &stage : cascade.stages) {
		QDomElement stageElement = doc.createElement("_");
		stagesElement.appendChild(stageElement);
		
		// Add stage threshold
		QDomElement stageThresholdElement = doc.createElement("stage_threshold");
		stageThresholdElement.appendChild(doc.createTextNode(QString::number(stage.stageThreshold)));
		stageElement.appendChild(stageThresholdElement);
		
		// Add weak classifiers
		QDomElement weakClassifiersElement = doc.createElement("weakClassifiers");
		stageElement.appendChild(weakClassifiersElement);
		
		for (const HaarWeakClassifier &classifier : stage.weakClassifiers) {
			QDomElement classifierElement = doc.createElement("_");
			weakClassifiersElement.appendChild(classifierElement);
			
			// Add internalNodes
			QDomElement internalNodesElement = doc.createElement("internalNodes");
			QString internalNodesText = QString("%1 %2 %3")
											.arg(classifier.leftChild)
											.arg(classifier.rightChild)
											.arg(classifier.threshold);
			internalNodesElement.appendChild(doc.createTextNode(internalNodesText));
			classifierElement.appendChild(internalNodesElement);
			
			// Add leafValues
			QDomElement leafValuesElement = doc.createElement("leafValues");
			QString leafValuesText = QString("%1 %2").arg(classifier.leftVal).arg(classifier.rightVal);
			leafValuesElement.appendChild(doc.createTextNode(leafValuesText));
			classifierElement.appendChild(leafValuesElement);
		}
	}
	
	// Write XML to file
	QFile file(destination);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qFatal("Failed to open XML file for writing: %s", qPrintable(destination));
	}
	
	QTextStream stream(&file);
	stream << doc.toString();
	file.close();
	
	qDebug() << "Haar cascade saved to XML file:" << destination;
}

