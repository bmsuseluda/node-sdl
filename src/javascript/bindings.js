const Path = require("path");

// sdl2-compat loads SDL3.dll at runtime by name, so its directory must be on the DLL search path
if (process.platform === "win32") {
  const distDir = Path.resolve(__dirname, "../../dist");
  const pathKey =
    Object.keys(process.env).find((key) => key.toUpperCase() === "PATH") ||
    "PATH";
  const current = process.env[pathKey] || "";
  if (!current.split(Path.delimiter).includes(distDir)) {
    process.env[pathKey] = `${distDir}${Path.delimiter}${current}`;
  }
}

module.exports = require("../../dist/sdl.node");
