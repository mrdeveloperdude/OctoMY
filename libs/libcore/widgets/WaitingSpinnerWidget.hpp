#ifndef WAITINGSPINNERWIDGET_HPP
#define WAITINGSPINNERWIDGET_HPP


/* Original Work Copyright (c) 2012-2014 Alexander Turkin
   Modified 2014 by William Hallatt
   Modified 2015 by Jacob Dawid
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Qt includes
#include <QWidget>
#include <QTimer>
#include <QColor>



class SpinnerStyle{
	private:
		QColor  mColor;
		qreal   mRoundness;
		qreal   mMinimumTrailOpacity;
		qreal   mTrailFadePercentage;
		qreal   mRevolutionsPerSecond;
		int     mNumberOfLines;
		qreal     mLineLength;
		qreal     mLineWidth;
		qreal     mInnerRadius;
		bool mRelativeSizes;

	public:

		SpinnerStyle()
			: mColor(Qt::white)
			, mRoundness(100.0)
			, mMinimumTrailOpacity(M_PI)
			, mTrailFadePercentage(80.0)
			, mRevolutionsPerSecond(M_PI/2)
			, mNumberOfLines(20)
			, mLineLength(10)
			, mLineWidth(2)
			, mInnerRadius(10)
		{

		}

	public:
		void setColor(QColor color);
		void setRoundness(qreal roundness);
		void setMinimumTrailOpacity(qreal minimumTrailOpacity);
		void setTrailFadePercentage(qreal trail);
		void setRevolutionsPerSecond(qreal revolutionsPerSecond);
		void setNumberOfLines(int lines);
		void setLineLength(qreal length);
		void setLineWidth(qreal width);
		void setInnerRadius(qreal radius);
		void setRelatveSize(bool realtive);

	public:

		inline QColor color() const
		{
			return mColor;
		}

		inline qreal roundness() const
		{
			return mRoundness;
		}

		inline qreal minimumTrailOpacity() const
		{
			return mMinimumTrailOpacity;
		}

		inline qreal trailFadePercentage() const
		{
			return mTrailFadePercentage;
		}

		inline qreal revolutionsPerSecond() const
		{
			return mRevolutionsPerSecond;
		}

		inline int numberOfLines() const
		{
			return mNumberOfLines;
		}

		inline qreal lineLength() const
		{
			return mLineLength;
		}

		inline qreal lineWidth() const
		{
			return mLineWidth;
		}

		inline qreal innerRadius() const
		{
			return mInnerRadius;
		}

		inline bool relativeSizes() const
		{
			return mRelativeSizes;
		}

		friend class WaitingSpinnerWidget;
};

class WaitingSpinnerWidget : public QWidget {
		Q_OBJECT
	public:
		/*! Constructor for "standard" widget behaviour - use this
   * constructor if you wish to, e.g. embed your widget in another. */
		WaitingSpinnerWidget(QWidget *parent = 0,
							 bool centerOnParent = false,
							 bool disableParentWhenSpinning = false);

		/*! Constructor - use this constructor to automatically create a modal
   * ("blocking") spinner on top of the calling widget/window.  If a valid
   * parent widget is provided, "centreOnParent" will ensure that
   * QtWaitingSpinner automatically centres itself on it, if not,
   * "centreOnParent" is ignored. */

		/*
		WaitingSpinnerWidget(Qt::WindowModality modality,
							 QWidget *parent = 0,
							 bool centerOnParent = false,
							 bool disableParentWhenSpinning = false);
*/

	private:

		SpinnerStyle mStyle;

		QTimer mTimer;
		bool    mCenterOnParent;
		bool    mDisableParentWhenSpinning;
		int     mCurrentCounter;
		bool    mIsSpinning;



	public:

		inline void setStyle(const SpinnerStyle &style){
			mStyle=style;
			mCurrentCounter = 0;
			updateTimer();
			updateSize();
		}


		inline const SpinnerStyle &style(){
			return mStyle;
		}


		void setText(QString text);
		bool isSpinning() const;


	protected:
		void paintEvent(QPaintEvent *paintEvent);

	private:
		static int lineCountDistanceFromPrimary(int current, int primary,
												int totalNrOfLines);
		static QColor currentLineColor(int distance, int totalNrOfLines,
									   qreal trailFadePerc, qreal minOpacity,
									   QColor color);

		void initialize();
		void updateSize();
		void updateTimer();
		void updatePosition();


	private:
		WaitingSpinnerWidget(const WaitingSpinnerWidget&);
		WaitingSpinnerWidget& operator=(const WaitingSpinnerWidget&);

	private slots:
		void rotate();

	public slots:
		void start();
		void stop();


};

#endif // WAITINGSPINNERWIDGET_HPP
