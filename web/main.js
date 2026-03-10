var Module = {
  canvas: document.getElementById('canvas'),
  print: (...args) => console.log(args.join(' ')),
  printErr: (...args) => console.error(args.join(' ')),
  setStatus: (text) => { if (text) console.log("Emscripten Status: " + text); }
};

window.onerror = () => {
  console.error("Exception thrown, see JavaScript console");
};
