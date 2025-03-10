Product{
	name: "Meta"
	Group{
		name: "Meta Files"
		prefix: "../../"
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
						, "**/MANDATE*"
						, "**/LICENSE*"
						, "**/VERSION*"
						, "**/CHANGE*"
			];
			if(debug){
				console.info("Meta Files:" + name);
				console.info(" + prefix:" + prefix);
				console.info(" + excludeFiles:" + excludeFiles);
				console.info(" + files:" + f);
			}
			return f;
		}
	}
}

