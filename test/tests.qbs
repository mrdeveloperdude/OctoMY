import qbs.FileInfo

Project{
	name: "Tests (selection)"

	references: {
		var ret = [
//			  "testMarkdownEditor/test.qbs"
//			, "testPlanEditor/test.qbs"
			  "testServiceManager/test.qbs"
			, "testMetaBlobsWidget/test.qbs"
//			, "testWizardWidget/test.qbs"
//			  "testDiscovery/test.qbs"
//			, "testFork/test.qbs"
		 ];
		console.log("tests selection");
		console.log(ret);
		return ret;
	}
	/*
	AutotestRunner { 
		workingDir:{
			console.log("LOG");
			console.info("AutotestRunner: "+limitToSubProject);
			return undefined;
		}
		limitToSubProject:false
	}*/
}

