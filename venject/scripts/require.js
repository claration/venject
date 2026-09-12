// This redirects the relaunch function to launch the modloader executable instead of the original executable
// There is a bug on Linux where Discord does not relaunch. This is an electron bug, and I haven't figured out how to fix it yet.
// See: https://github.com/electron/electron/issues/41463
if (process.env.CLIENT_EXECUTABLE_PATH) {
	const { app } = require("electron");
	const _relaunch = app.relaunch;

	app.relaunch = (options = {}) => {
		let args = process.argv.slice(1);

		try {
			args = JSON.parse(process.env.CLIENT_ARGV_JSON);
		} catch (e) {
			console.error("Failed to parse CLIENT_ARGV_JSON", e);
		}

		_relaunch.call(app, {
			args,
			execPath: process.env.CLIENT_EXECUTABLE_PATH,
		});
	};
}

require(process.env.CLIENT_JS_PATH);
