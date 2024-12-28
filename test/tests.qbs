import qbs.FileInfo

Project{
	name: "Tests (selection)"

	references: {
		var ret = [
			  "testTest/test.qbs"
//			, "testMarkdownEditor/test.qbs"
//			, "testPlanEditor/test.qbs"
//			, "testServiceManager/test.qbs"
//			, "testController/test.qbs"
//			, "testServotor32Controller/test.qbs"
//			, "testAgentConfig/test.qbs"
			, "testEval/test.qbs"
			, "testMarchingCubesWidget/test.qbs"
			, "testFace/test.qbs"
//			, "testEditableLabel/test.qbs"
//			, "testActuatorWidget/test.qbs"
//			, "testDecimalEntry/test.qbs"
//			, "testNoticeList/test.qbs"
//			, "testSyncParameter/test.qbs"
//			, "testSync/test.qbs"
//			, "testQr/test.qbs"
//			, "testMetaBlobsWidget/test.qbs"
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

