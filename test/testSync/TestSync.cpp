#include "TestSync.hpp"
#include "sync/Sync.hpp"

void TestSync::test()
{
	// Define the schema using the templated constructor to set type IDs
	SyncSchema schema;
	schema.entries = {
		SyncSchemaEntry::createEntry<bool>("testBool", "A boolean value", 1),
		SyncSchemaEntry::createEntry<qint8>("testInt8", "8-bit signed integer", 8),
		SyncSchemaEntry::createEntry<quint8>("testUInt8", "8-bit unsigned integer", 8),
		SyncSchemaEntry::createEntry<float>("testFloat32", "32-bit float", 32),
		SyncSchemaEntry::createEntry<double>("testDouble64", "64-bit double", 64),
		SyncSchemaEntry::createEntry<QString>("testString", "A variable-length string", 0)
	};

	QByteArray encodedData;
	SyncClient sourceClient;
	QVector<SyncValue> sourceValues;
	
	// Setting up the source client and encoding values
	{
		sourceClient.schema = schema;
		sourceValues = {
			{ true },
			{ static_cast<qint8>(-128) },
			{ static_cast<quint8>(255) },
			{ static_cast<float>(1.23f) },
			{ static_cast<double>(3.14) },
			{ QString("Hello, World!") }
		};
		sourceClient.setValues(sourceValues);
		
		// Encode data from source client
		const bool encodeStatus = sourceClient.encode(encodedData);
		qDebug() << "Encode error (if any):" << sourceClient.errorString();
		QCOMPARE(encodeStatus, true);
		qDebug() << "Encoded size:" << encodedData.size();
	}
	
	// Decoding and validation
	{
		// Instantiate target client with the same schema but no values
		SyncClient targetClient;
		targetClient.schema = schema;
		
		// Decode data into target client
		const bool decodeStatus = targetClient.decode(encodedData);
		qDebug() << "Decode error (if any):" << targetClient.errorString();
		QCOMPARE(decodeStatus, true);
		
		// Validate decoded values to ensure they match the original ones
		const auto targetValues = targetClient.values();
		for (int i = 0; i < targetValues.size(); ++i) {
			const auto &sourceValue = sourceValues[i];
			const auto &targetValue = targetValues[i];
			const auto &schemaEntry = schema.entries[i];
			
			QCOMPARE(sourceValue.value, targetValue.value);
			qDebug() << "Source value:" << sourceValue << "Target value:" << targetValue << ", Schema entry:" << schemaEntry;
		}
	}
}

// Macro to register the test with your test framework
OC_TEST_MAIN(test, TestSync)
