import qbs

Project {
	name: "Canal"
	references: [
		"app/app.qbs",
		"plurq/plurq.qbs",
        "vendor/qtkeychain/qtkeychain.qbs",
	]
}
