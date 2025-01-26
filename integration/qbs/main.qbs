import qbs.FileInfo

Project {
	name: "Main"
	//AndroidOptions { }

	references: {
		const f = [
					"../../src/libs/libcombined/libs_combined.qbs" // All libs combined into on
//					  "../../src/libs.qbs" // Individual libs included to project one by one
					, "../../test/tests.qbs"
					, "../../integration/qbs/meta.qbs"
					, "../../src/apps.qbs"
				];
		console.info("Main:");
		console.info(" + references: " + f);
		return f;
	}
}
