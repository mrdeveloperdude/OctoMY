import qbs.FileInfo

Module {
	Files{
		name: "unnamed_shader_sources"
		types: ['vert', 'frag', 'geom', 'comp', 'tesc', 'tese']
		fileTags: ["shader_source"]
		baseDir: project.sourcePath
	}
	Rule {
		inputs: ["shader_source"]
		Artifact {
			filePath: input.fileName + ".qsb"
			fileTags: ["shader_object", "qt.core.resource_data"]
		}
		prepare: {
			var debug = true;
			var qsbToolPath = FileInfo.joinPaths(product.Qt.core.binPath, "qsb");
			var cmd = new Command(qsbToolPath, [
				  input.filePath
				, "-o", output.filePath
				, "--glsl", "450"
				]);
			cmd.description = "Compiling shader " + input.fileName + " to " + output.filePath;
			cmd.highlight = "codegen";
			if(debug){
				console.info("desc: " + cmd.description);
				console.info("Using qsb tool at: " + qsbToolPath);
			}
			return [cmd];
		}
	}
}
