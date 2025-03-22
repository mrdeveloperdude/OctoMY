import qbs.FileInfo

Project{
	name: "Tests (selection)"

	references: {
		var ret = [
			  "testTest/test.qbs"
//			, "testD3/test.qbs"
//			, "testEval/test.qbs"
//			, "testFace/test.qbs"
//			, "testHaar/test.qbs"
//			, "testSplitter/test.qbs"
//			, "testCommsSession/test.qbs"
//			, "testCommsChannel/test.qbs"
//			, "testCommsCarrier/test.qbs"
//			, "testCarrierAddress/test.qbs"
//			, "testSplashScreen/test.qbs"
//			, "testDiscovery/test.qbs"
//			, "testPingWidget/test.qbs"
			, "testTrigger/test.qbs"
			, "testScreen/test.qbs"
			, "testFlowLayout/test.qbs"
			, "testMenuActivity/test.qbs"
//			  "testDiscovery/test.qbs"
//			, "testActuatorWidget/test.qbs"
//			, "testAgentConfig/test.qbs"
//			, "testController/test.qbs"
//			, "testDecimalEntry/test.qbs"
//			, "testEditableLabel/test.qbs"
//			, "testFork/test.qbs"
//			, "testMarchingCubesWidget/test.qbs"
//			, "testMarkdownEditor/test.qbs"
//			, "testMetaBlobsWidget/test.qbs"
//			, "testNoticeList/test.qbs"
//			, "testPlanEditor/test.qbs"
//			, "testQr/test.qbs"
//			, "testServiceManager/test.qbs"
//			, "testServotor32Controller/test.qbs"
//			, "testSync/test.qbs"
//			, "testSyncParameter/test.qbs"
//			, "testWizardWidget/test.qbs"
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

