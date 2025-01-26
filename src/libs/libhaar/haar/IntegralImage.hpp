#ifndef INTEGRALIMAGE_HPP
#define INTEGRALIMAGE_HPP
#include <vector>
#include <QSize>
#include <QRect>
#include <QImage>


inline constexpr int bt601_gray(int r, int g, int b)         // convert R,G,B to gray 0..255
{
	// Scale weights by 1000 for integer arithmetic
	return (r * 299 + g * 587 + b * 114) / 1000;
}

inline constexpr int bt601_gray(QRgb rgb)                    // convert RGB to gray 0..255
{
	return bt601_gray(qRed(rgb), qGreen(rgb), qBlue(rgb));
}


template <typename T>
class IntegralImage {
private:
	QSize m_size;
	std::vector<T> m_data;

public:
	IntegralImage(const QSize &size = QSize()) : m_size(size), m_data(size.width() * size.height(), 0) {}
	IntegralImage(const QImage image) : IntegralImage(image.size()) {set(image);}
	QSize size() const { return m_size; }
	int width() const { return m_size.width(); }
	int height() const { return m_size.height(); }
	void set(const QImage &grayscale) {
		if (grayscale.size() != m_size) {
			m_size = grayscale.size();
			m_data.resize(m_size.width() * m_size.height(), 0);
		}
		for (int y = 0; y < m_size.height(); ++y) {
			T rowSum = 0;
			for (int x = 0; x < m_size.width(); ++x) {
				rowSum += bt601_gray(grayscale.pixel(x, y));
				m_data[y * width() + x] = rowSum + (y > 0 ? m_data[(y - 1) * width() + x] : 0);
			}
		}
	}
	T sumInRect(const QRect &rect) const {
		int x1 = std::max(0, rect.left());
		int y1 = std::max(0, rect.top());
		int x2 = std::min(rect.right(), width() - 1);
		int y2 = std::min(rect.bottom(), height() - 1);
		
		if (x1 > x2 || y1 > y2) {
			return 0;
		}
		
		T A = (x1 > 0 && y1 > 0) ? m_data[(y1 - 1) * width() + (x1 - 1)] : 0;
		T B = (y1 > 0) ? m_data[(y1 - 1) * width() + x2] : 0;
		T C = (x1 > 0) ? m_data[y2 * width() + (x1 - 1)] : 0;
		T D = m_data[y2 * width() + x2];

		return D - B - C + A;
	}
};


#endif // INTEGRALIMAGE_HPP
