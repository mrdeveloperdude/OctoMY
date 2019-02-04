#include "OctoStyle.hpp"
#include "uptime/New.hpp"

#include <QStylePlugin>

class OctoStylePlugin : public QStylePlugin
{
	public:
		QStringList keys() const {
			return QStringList(QLatin1String("Octo"));
		}

		QStyle *create(const QString &key) {
			if (key.toLower() == QLatin1String("octo")) {
				return OC_NEW OctoStyle;
			}
			return nullptr;
		}
};
