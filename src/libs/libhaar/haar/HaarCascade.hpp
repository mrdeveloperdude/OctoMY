#ifndef HAARCASCADE_HPP
#define HAARCASCADE_HPP

#include <QImage>
#include <QRect>
#include <QSize>
#include <QVector>

class QPainter;
class QImage;

// Data structure for a single rectangle with weight
struct HaarRect {
	QRect region;   // Rectangle region
	double weight;  // Weight of the rectangle
};

// Data structure for a single Haar feature
struct HaarFeature {
	QVector<HaarRect> rects;  // List of rectangles with weights
};

// Data structure for a weak classifier
struct HaarWeakClassifier {
	int featureIndex;   // Index into the features vector
	int leftChild;      // Index of the left child (-1 for leaf node)
	int rightChild;     // Index of the right child (-1 for leaf node)
	double threshold;   // Threshold for evaluation
	double leftVal;     // Value if the weak classifier evaluates to false
	double rightVal;    // Value if the weak classifier evaluates to true
};

// Data structure for a single stage in the Haar cascade
struct HaarStage {
	QVector<HaarWeakClassifier> weakClassifiers; // Weak classifiers in this stage
	double stageThreshold;                       // Threshold for this stage
};

// Data structure for the Haar cascade
struct HaarCascade {
	QString filename;           // File name of the cascade
	QString name;               // Human-readable name of the cascade
	QSize windowSize;           // Detection window size (width, height)
	QVector<HaarFeature> features;  // Shared features used across stages
	QVector<HaarStage> stages;      // Stages of the cascade
	QImage preview;             // Preview image for visualization
};

QDebug operator<<(QDebug debug, const HaarRect &rect);
QDebug operator<<(QDebug debug, const HaarFeature &feature);
QDebug operator<<(QDebug debug, const HaarStage &stage);
QDebug operator<<(QDebug debug, const HaarCascade &cascade);

#endif // HAARCASCADE_HPP
