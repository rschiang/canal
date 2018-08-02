import qbs

Product {
    name: "qtkeychain"
    property bool plaintextStore: false
    property bool buildTranslations: false
    property bool buildStatic: false
    property bool libsecretSupport: true

    type: buildStatic ? "staticlibrary" : "dynamiclibrary"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    cpp.defines: [buildStatic ? "QKEYCHAIN_STATICLIB" : "QKEYCHAIN_SHAREDLIB"]

    Group {
        name: "Source"
        files: ["keychain.cpp", "keychain.h", "keychain_p.h"]
    }

    Group {
        condition: plaintextStore
        name: "PlainTextStore"
        files: ["plaintextstore.cpp", "plaintextstore_p.h"]
    }

    Group {
        condition: buildTranslations
        name: "Translations"
        files: ["translations/*.ts"]
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    // Generic Linux configuration

    Depends {
        condition: qbs.targetOS.contains("linux")
        name: "Qt.dbus"
    }

    Group {
        condition: qbs.targetOS.contains("linux")
        name: "Linux Support"
        files: ["keychain_unix.cpp", "gnomekeyring.cpp", "gnomekeyring_p.h"]
    }

    // Libsecret support

    Properties {
        condition: qbs.targetOS.contains("linux") && libsecretSupport
        cpp.defines: outer.concat(["HAVE_LIBSECRET=1"])
    }

    // Windows configuration

    Properties {
        condition: qbs.targetOS.contains("windows") && plaintextStore
        cpp.defines: outer.concat(["USE_CREDENTIAL_STORE=1"])
        cpp.dynamicLibraries: ["Crypt32"]
    }

    Group {
        condition: qbs.targetOS.contains("windows")
        name: "Windows"
        files: ["keychain_win.cpp", "libsecret.cpp", "libsecret_p.h"]
    }

    // macOS / iOS configuration

    Properties {
        condition: qbs.targetOS.contains("macos") || qbs.targetOS.contains("ios")
        cpp.frameworks: ["Security", "Foundation"]
    }

    Group {
        condition: qbs.targetOS.contains("macos")
        name: "macOS"
        files: ["keychain_mac.cpp"]
    }

    Group {
        condition: qbs.targetOS.contains("ios")
        name: "iOS"
        files: ["keychain_ios.mm"]
    }
}
