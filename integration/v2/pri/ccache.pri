gcc{

# Automatic support for ccache
CCACHE_EXISTS=$$system("which ccache >/dev/null 2>&1; echo $?")
equals(CCACHE_EXISTS, "0"){
	QMAKE_CXX="ccache $$QMAKE_CXX"
	#message("GOT CCACHE: $$QMAKE_CXX")
}

#message("QMAKE_CXX: $$QMAKE_CXX, CCACHE_EXISTS: $$CCACHE_EXISTS")
}
