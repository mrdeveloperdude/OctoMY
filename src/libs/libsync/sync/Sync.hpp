#ifndef SYNC_HPP
#define SYNC_HPP

#include <QByteArray>
#include <QDebug>
#include <QIODevice>
#include <QString>
#include <QVariant>
#include <QVector>

#include <variant>


template<typename T>
struct TypeID;


template<>
struct TypeID<bool> {
	static constexpr int value = 1;
};

template<>
struct TypeID<qint8> {
	static constexpr int value = 81;
};

template<>
struct TypeID<quint8> {
	static constexpr int value = 80;
};

template<>
struct TypeID<qint16> {
	static constexpr int value = 161;
};

template<>
struct TypeID<quint16> {
	static constexpr int value = 160;
};

template<>
struct TypeID<qint32> {
	static constexpr int value = 321;
};

template<>
struct TypeID<quint32> {
	static constexpr int value = 320;
};

template<>
struct TypeID<qint64> {
	static constexpr int value = 641;
};

template<>
struct TypeID<quint64> {
	static constexpr int value = 640;
};

template<>
struct TypeID<float> {
	static constexpr int value = 6410;
};

template<>
struct TypeID<double> {
	static constexpr int value = 8010;
};

template<>
struct TypeID<QString> {
	static constexpr int value = 10000;
};


struct SyncSchemaEntry {
	QString name;
	QString description;
	quint8 bits; // Number of bits to encode for integer types
	int typeId;  // Compile-time type ID
	
	// Templated constructor to deduce and store the type ID
	template<typename T>
	SyncSchemaEntry(const QString &name, const QString &description, quint8 bits)
		: name(name), description(description), bits(bits), typeId(TypeID<T>::value) {}
	
	// Non-templated constructor to initialize with typeId
	SyncSchemaEntry(const QString &name, const QString &description, quint8 bits, int typeId)
		: name(name), description(description), bits(bits), typeId(typeId) {}
	
	template<typename T>
	static SyncSchemaEntry createEntry(const QString &name, const QString &description, quint8 bits) {
		return SyncSchemaEntry(name, description, bits, TypeID<T>::value);
	}
};


// Use std::variant to handle different types safely
using SyncUnion = std::variant<
	bool,
	qint8, quint8,
	qint16, quint16,
	qint32, quint32,
	qint64, quint64,
	float, double,
	QString
	>;


struct SyncValue {
	SyncUnion value;
	// Default constructor
	SyncValue() = default;
	// Constructor for convenience
	SyncValue(const SyncUnion &value) : value(value) {}
};

struct SyncSchema {
	QVector<SyncSchemaEntry> entries;
};

class SyncClient {
private:
	QString mErrorString;
	QVector<SyncValue> mValues; // The actual values being encoded/decoded
	
public:
	SyncSchema schema;
	
	bool encode(QByteArray &out);
	bool decode(const QByteArray &in);
	QString errorString() const { return mErrorString; }
	
	// Setter for values
	void setValues(const QVector<SyncValue> &newValues) {
		mValues = newValues;
	}
	
	// Getter for values
	QVector<SyncValue> values() const {
		return mValues;
	}
};

// Helper function to write bits to a byte array
void writeBits(QByteArray &out, quint64 value, quint8 bitCount, quint8 &bitOffset, quint8 &currentByte) {
	for (quint8 i = 0; i < bitCount; ++i) {
		if (value & (1ULL << i)) {
			currentByte |= (1 << bitOffset);
		}
		bitOffset++;
		
		if (bitOffset == 8) {
			out.append(static_cast<char>(currentByte));
			currentByte = 0;
			bitOffset = 0;
		}
	}
}

// Helper function to read bits from a byte array
quint64 readBits(const QByteArray &in, quint8 bitCount, quint32 &byteIndex, quint8 &bitOffset, QString &errorString) {
	quint64 result = 0;
	quint8 currentByte = 0;
	
	for (quint8 i = 0; i < bitCount; ++i) {
		if (bitOffset == 0 && byteIndex < static_cast<quint32>(in.size())) {
			currentByte = static_cast<quint8>(in[byteIndex++]);
		}
		
		if (bitOffset >= 8 || byteIndex > static_cast<quint32>(in.size())) {
			errorString = "Attempted to read beyond available data.";
			return 0;
		}
		
		if (currentByte & (1 << bitOffset)) {
			result |= (1ULL << i);
		}
		bitOffset = (bitOffset + 1) % 8;
	}
	
	return result;
}
bool SyncClient::encode(QByteArray &out) {
	mErrorString.clear();
	out.clear();
	quint8 currentByte = 0;
	quint8 bitOffset = 0;
	
	if (schema.entries.size() != mValues.size()) {
		mErrorString = "Mismatch between schema entries and values.";
		return false;
	}
	
	for (int i = 0; i < schema.entries.size(); ++i) {
		const auto &entry = schema.entries[i];
		const auto &syncValue = mValues[i];
		
		bool result = std::visit([&out, &bitOffset, &currentByte, &entry, this](auto &&arg) -> bool {
			using T = std::decay_t<decltype(arg)>;
			if (entry.typeId == TypeID<T>::value) {
				if constexpr (std::is_integral_v<T> || std::is_same_v<T, bool>) {
					quint64 valueToPack = static_cast<quint64>(arg);
					writeBits(out, valueToPack, entry.bits, bitOffset, currentByte);
					return true;
				} else if constexpr (std::is_floating_point_v<T>) {
					const char *data = reinterpret_cast<const char *>(&arg);
					for (size_t i = 0; i < sizeof(T); ++i) {
						writeBits(out, static_cast<quint8>(data[i]), 8, bitOffset, currentByte);
					}
					return true;
				} else if constexpr (std::is_same_v<T, QString>) {
					QByteArray strBytes = arg.toUtf8();
					writeBits(out, strBytes.size(), 32, bitOffset, currentByte);
					for (char byte : strBytes) {
						writeBits(out, static_cast<quint8>(byte), 8, bitOffset, currentByte);
					}
					return true;
				}
			} else {
				mErrorString = QString("Type mismatch for entry '%1'").arg(entry.name);
				return false;
			}
			
			mErrorString = QString("Unsupported type for entry '%1'").arg(entry.name);
			return false;
		}, syncValue.value);
		
		if (!result) {
			return false;
		}
	}
	
	if (bitOffset > 0) {
		out.append(static_cast<char>(currentByte));
	}
	
	return true;
}

bool SyncClient::decode(const QByteArray &in) {
	mErrorString.clear();
	quint32 byteIndex = 0;
	quint8 bitOffset = 0;
	mValues.clear();
	
	for (const auto &entry : schema.entries) {
		SyncValue decodedValue;
		
		bool result = std::visit([&in, &byteIndex, &bitOffset, &entry, &decodedValue, this](auto &&arg) mutable -> bool {
			using T = std::decay_t<decltype(arg)>;
			
			// Check type ID for matching
			if (entry.typeId == TypeID<T>::value) {
				if constexpr (std::is_same_v<T, bool>) {
					quint8 temp = readBits(in, entry.bits, byteIndex, bitOffset, mErrorString);
					if (!mErrorString.isEmpty()) {
						return false;
					}
					decodedValue.value = static_cast<bool>(temp);
					return true;
				} else if constexpr (std::is_integral_v<T>) {
					quint64 temp = readBits(in, entry.bits, byteIndex, bitOffset, mErrorString);
					if (!mErrorString.isEmpty()) {
						return false;
					}
					decodedValue.value = static_cast<T>(temp);
					return true;
				} else if constexpr (std::is_floating_point_v<T>) {
					T val = 0;
					char *data = reinterpret_cast<char *>(&val);
					for (size_t i = 0; i < sizeof(T); ++i) {
						data[i] = static_cast<char>(readBits(in, 8, byteIndex, bitOffset, mErrorString));
						if (!mErrorString.isEmpty()) {
							return false;
						}
					}
					decodedValue.value = val;
					return true;
				} else if constexpr (std::is_same_v<T, QString>) {
					quint32 length = readBits(in, 32, byteIndex, bitOffset, mErrorString);
					if (!mErrorString.isEmpty()) {
						return false;
					}
					
					if (byteIndex + length > static_cast<quint32>(in.size())) {
						mErrorString = "String length exceeds available data.";
						return false;
					}
					
					QByteArray strBytes;
					for (quint32 i = 0; i < length; ++i) {
						strBytes.append(static_cast<char>(readBits(in, 8, byteIndex, bitOffset, mErrorString)));
						if (!mErrorString.isEmpty()) {
							return false;
						}
					}
					decodedValue.value = QString::fromUtf8(strBytes);
					return true;
				}
			}
			
			mErrorString = QString("Unsupported or mismatched type in entry '%1'").arg(entry.name);
			return false;
		}, SyncUnion{});
		
		if (!result) {
			return false;
		}
		
		mValues.append(decodedValue);
	}
	
	return true;
}


QDebug operator<<(QDebug debug, const SyncUnion &value) {
	QDebugStateSaver saver(debug);
	std::visit([&debug](auto &&arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, QString>) {
			debug << arg;
		} else {
			debug << QVariant::fromValue(arg).toString();
		}
	}, value);
	
	return debug;
}

QDebug operator<<(QDebug debug, const SyncValue &syncValue) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "SyncValue(" << syncValue.value << ")";
	return debug;
}


QDebug operator<<(QDebug debug, const SyncSchemaEntry &entry) {
	QDebugStateSaver saver(debug);
	debug.nospace() << "SyncSchemaEntry(" << entry.name << ", " << entry.description << ", " << entry.bits << ")";
	return debug;
}

#endif // SYNC_HPP
