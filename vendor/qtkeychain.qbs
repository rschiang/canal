import qbs

Product {
    name: "qtkeychain"
    property boolean plaintextStore: false
    property boolean buildTranslations: false
    property boolean buildStatic: false
    property boolean libsecretSupport: true

    type: buildStatic ? "staticlibrary" : "dynamiclibrary"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    cpp.cxxLanguageVersion: "c++11"

    Group {
        prefix: "qtkeychain/"
        files: ["keychain.cpp", "keychain.h", "keychain_p.h"]
    }

    Group {
        condition: plaintextStore
        prefix: "qtkeychain/"
        files: ["plaintextstore.cpp", "plaintextstore_p.h"]
    }

    Group {
        condition: buildTranslations
        prefix: "qtkeychain/"
        files: ["translations/*.ts"]
    }

    Export {
        cpp.incluePaths: product.sourceDirectory + "qtkeychain/"
    }

    // Generic Linux configuration

    Depends {
        condition: qbs.targetOS.contains("linux")
        name: "Qt.dbus"
    }

    Group {
        condition: qbs.targetOS.contains("linux")
        prefix: "qtkeychain/"
        files: ["keychain_unix.cpp", "gnomekeyring.cpp", "gnomekeyring_p.h"]
    }

    // Libsecret support

    Properties {
        condition: qbs.targetOS.contains("linux") && libsecretSupport
        cpp.defines: ["HAVE_LIBSECRET=1"]
    }

    // Windows configuration

    Properties {
        condition: qbs.targetOS.contains("windows") && plaintextStore
        cpp.defines: ["USE_CREDENTIAL_STORE=1"]
        cpp.dynamicLibraries: ["Crypt32"]
    }

    Group {
        condition: qbs.targetOS.contains("windows")
        prefix: "qtkeychain/"
        files: ["keychain_win.cpp", "libsecret.cpp", "libsecret_p.h"]
    }

    // macOS / iOS configuration

    Properties {
        condition: qbs.targetOS.contains("macos") || qbs.targetOS.contains("ios")
        cpp.frameworks: ["Security", "Foundation"]
    }

    Group {
        condition: qbs.targetOS.contains("macos")
        prefix: "qtkeychain/"
        files: ["keychain_mac.cpp"]
    }

    Group {
        condition: qbs.targetOS.contains("ios")
        prefix: "qtkeychain/"
        files: ["keychain_ios.mm"]
    }
}
