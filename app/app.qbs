import qbs

QtGuiApplication {
    name: "Canal"

    Depends { name: "Plurq" }
    Depends { name: "qtkeychain" }
    Depends {
        name: "Qt"
        submodules: ["quick", "widgets", "network", "networkauth"]
    }

    files: [
        "qml/main.qrc",
        "res/resources.qrc",
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
