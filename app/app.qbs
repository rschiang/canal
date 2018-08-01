import qbs

QtGuiApplication {
    name: "Canal"

    Depends { name: "Plurq" }
    Depends {
        name: "Qt"
        submodules: ["quick", "network", "networkauth"]
    }

    files: [
        "qml/main.qrc",
    ]

    Group {
        name: "QML Files"
        files: "qml/*.qml"
    }

    Group {
        name: "Source"
        files: ["*.cpp", "*.h"]
    }
}
