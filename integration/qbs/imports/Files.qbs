Group{
	name: "unnamed"
	prefix: ""
	excludeFiles: "/**/internal/**/*"
	property stringList types: []
	property stringList bases: ['/**/*.', '/*.']
	property bool debug: false
	files: {
		const _files = bases.reduce(function(acc, _base) {
			types.forEach(function(_type) {
				acc.push(_base + _type);
			});
			return acc;
		}, []);

		if(debug){
			console.info("Files for " + name);
			console.info(" + prefix:" + prefix);
			console.info(" + excludeFiles:" + excludeFiles);
			console.info(" + files:" + JSON.stringify(_files, null, "\t"));
		}
		return _files;
	}
}
