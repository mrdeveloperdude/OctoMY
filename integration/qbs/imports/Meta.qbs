Group{
	name: "Meta Files"
	prefix: "sdfsdfsdfsedf"
	excludeFiles: "**/internal/**/*"
	property bool debug: true
	files: {
		const f = [
					  "**/*.qbs"
					, "**/*.env"
					, "**/*.pro"
					, "**/*.pri"
					, "**/Docker*"
					, "**/docker-*"
					, "**/Make*"
					, "**/README*"
					, "**/VERSION*"
					, "**/CHANGE*"
		];
		if(debug){
			console.info("Meta Files:"+name);
			console.info(" + prefix:"+prefix);
			console.info(" + excludeFiles:"+excludeFiles);
			console.info(" + files:"+f);
		}
		return f;
	}
}
