#include "HaarCascade.hpp"

#include <QDebug>


QDebug operator<<(QDebug debug, const HaarRect &rect) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "HaarRect(region: " << rect.region << ", weight: " << rect.weight << ")";
	return debug;
}

QDebug operator<<(QDebug debug, const HaarFeature &feature) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "HaarFeature(rects: [";
	for (const HaarRect &rect : feature.rects) {
		debug.nospace() << rect << ", ";
	}
	debug.nospace() << "])";
	return debug;
}

QDebug operator<<(QDebug debug, const HaarWeakClassifier &weakClassifier) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "HaarWeakClassifier(featureIndex: " << weakClassifier.featureIndex
					<< ", threshold: " << weakClassifier.threshold
					<< ", leftChild: " << weakClassifier.leftChild
					<< ", rightChild: " << weakClassifier.rightChild
					<< ", leftVal: " << weakClassifier.leftVal
					<< ", rightVal: " << weakClassifier.rightVal << ")";
	return debug;
}

QDebug operator<<(QDebug debug, const HaarStage &stage) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "HaarStage(stageThreshold: " << stage.stageThreshold
					<< ", weakClassifiers: [";
	for (const HaarWeakClassifier &weakClassifier : stage.weakClassifiers) {
		debug.nospace() << weakClassifier << ", ";
	}
	debug.nospace() << "])";
	return debug;
}

QDebug operator<<(QDebug debug, const HaarCascade &cascade) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "HaarCascade(filename: " << cascade.filename
					<< ", name: " << cascade.name
					<< ", windowSize: " << cascade.windowSize
					<< ", features: " << cascade.features.size()
					<< ", stages: " << cascade.stages.size()
					<< ")";
	return debug;
}

