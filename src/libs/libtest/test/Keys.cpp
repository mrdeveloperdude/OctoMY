#include "Keys.hpp"

// Keys can conveniently be generated with this service (for testing only of course):
// http://travistidwell.com/jsencrypt/demo/

TestKeys::TestKeys():

	privKeyPEM("-----BEGIN RSA PRIVATE KEY-----\n"
			   "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\n"
			   "-----END RSA PRIVATE KEY-----\n")

	, pubKeyPEM("-----BEGIN PUBLIC KEY-----\n"
				"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAhfwrwmldif7ReZEqrMOd\n" "rDNQfgexYttfWjfHyReQzKiolDUrbVLBlSAGk/3R6MKfLjm+dHSJAQRyfp8daJMg\n"
				"YsYU3NQildb3mnjb3pevJC9DtSnX59aR9xyAuU5Ohm1LU0LcmL3S8p002YRL3U/0\n" "6g1ZDuW2UQU8uuvx8AtVnHN4IKhbHLCOaxdHJ8vItvVJ3wTAXMsG+iOWCFEzej/s\n"
				"6nN3qhNhQ3W0jWpC4d+Z6VIwUNWMq5Zz1HThZ9ZoSPYUIDivjhRaQtN5zrZyPjOz\n" "47r5etO4Rc07JCe7Gdvh6lx8coxA1DyAPVM6WEVcvfSucVVokd7lzvLcO3n41m/t\n"
				"WfRZsUkDR0MCdlRFycQ9LwF6ae/d/OD+vkZ4q0sobdwjqvZ85FUsIivxDACCpZHX\n" "t6tgLO74g0eWYRmfsq8JEEtTW0gjI7vpXoRsC2N0KWDAVN2Cw7f8qtneU9LyVDvx\n"
				"J69K0tn5nBj5aZFVVoDpaLLCy2SmGSf7x4HqUeE9daah1J9YFs7TMq7wZ9Cx/F2P\n" "pT8OvSPpX4uq1OLma5T0A0BxbnxJkmQxFPQJo0yXTgybY+GV82JRHvhAOmObNomv\n"
				"UgDsyr52AnWOI1ilJcX8EPSR7Jc5gLow8++j+SBrcZDDJ+fAIvmNO2a8wZjVWT+G\n"
				"GvVy7USArMzrGfjWcX90ugUCAwEAAQ==\n"
				"-----END PUBLIC KEY-----\n")
	, keyID("BC68E2093C30E393159B404B5A39F1AB7CE9E9FA452CD0F2C81563BB19F10DE5")
	// Hand-mutilated so that it is NOT valid
	, keyID_bad("BAD8E2093C30E393159B404B5A39F1AB7CE9E9FA452CD0F2C81563BB19F10DE5")

{

}
