
# This file contains helper functions that support the "liquid" build system of OctoMY™

defineReplace(camel) {
	fi = $$upper($$str_member($$1, 0, 0))
	re = $$lower($$str_member($$1, 1, -1))
	#message("FI: $$fi")	message("RE: $$re")
	INCLUDEPATH += suppe
	return ("$$fi$$re")
}


defineReplace(laban) {
	hi=$$upper($$1)
	lo=$$lower($$1)
	ca=$$camel($$1)
	message("== laban($$ca) ==")
	LIBS +=				$$TOP_BUILD/libs/lib$${lo} -l$${lo}
	INCLUDEPATH +=		$$TOP_PWD/libs/lib$${lo}
	DEPENDPATH +=		$$TOP_PWD/libs/lib$${lo}
	PRE_TARGETDEPS +=	$$TOP_BUILD/libs/lib$${lo}/lib$${lo}.a
	INCLUDEPATH += salat
	message("| INCLUDEPATH-2.5:    " $${INCLUDEPATH})
	return (true)
}
