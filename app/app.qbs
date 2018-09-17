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

    Depends {
        condition: qbs.targetOS.contains("macos")
        name: "bundle"
    }

    Depends {
        condition: qbs.targetOS.contains("macos")
        name: "ib"
    }

    Properties {
        condition: qbs.targetOS.contains("macos")
        cpp.dynamicLibraries: "objc"
        cpp.frameworks: "Foundation"
        bundle.identifierPrefix: "tw.poren"
        ib.appIconName: "AppIcon"
    }

    Group {
        name: "macOS"
        condition: qbs.targetOS.contains("macos")
        files: ["*.mm", "res/mac/resources.xcassets"]
    }
}
