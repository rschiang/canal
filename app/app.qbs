import qbs

QtGuiApplication {
    name: "Canal"
    Depends { name: "Qt.quick" }

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
