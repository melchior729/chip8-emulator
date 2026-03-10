/**
 * @file Contains the Emscripten module config and UI logic.
 * @author Abhay Manoj
 */

const urlParams = new URLSearchParams(window.location.search);
const romToLoad = urlParams.get('rom') || "roms/tetris.ch8";

var Module = {
  canvas: document.getElementById('canvas'),
  arguments: [romToLoad],
  print: (...args) => console.log(args.join(' ')),
  printErr: (...args) => console.error(args.join(' ')),
  setStatus: (text) => {
    if (text) console.log("Emscripten: " + text);
  },
  preRun: [function() {
    if (romToLoad === "upload.ch8") {
      const data = sessionStorage.getItem('custom_rom_bytes');
      if (data) {
        const bytes = new Uint8Array(JSON.parse(data));
        FS.writeFile('upload.ch8', bytes);
      }
    }
  }]
};

document.addEventListener('DOMContentLoaded', () => {
  const romSelector = document.getElementById('rom-selector');
  const resetBtn = document.getElementById('btn-reset');
  const fileInput = document.getElementById('file-input');
  const colorPicker = document.getElementById('draw-color');
  const descEl = document.getElementById('game-desc');
  const keyEl = document.getElementById('key-map');
  const panel = document.getElementById('instructions-panel');

  romSelector?.addEventListener('change', (e) => {
    window.location.href = `?rom=${encodeURIComponent(e.target.value)}`;
  });

  resetBtn?.addEventListener('click', () => {
    if (romToLoad === "upload.ch8") {
      window.location.href = window.location.pathname;
    } else {
      window.location.reload();
    }
  });

  fileInput?.addEventListener('change', function() {
    const file = this.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = function() {
      const bytes = Array.from(new Uint8Array(reader.result));
      sessionStorage.setItem('custom_rom_bytes', JSON.stringify(bytes));
      window.location.href = `?rom=upload.ch8`;
    };
    reader.readAsArrayBuffer(file);
  });

  colorPicker?.addEventListener('input', (e) => {
    const hex = e.target.value;

    const r = parseInt(hex.slice(1, 3), 16);
    const g = parseInt(hex.slice(3, 5), 16);
    const b = parseInt(hex.slice(5, 7), 16);

    // Call the C++ function exported via EMSCRIPTEN_KEEPALIVE
    if (typeof Module._set_draw_color === 'function') {
      Module._set_draw_color(r, g, b);
    } else {
      console.warn("Wasm bridge 'set_draw_color' not found yet.");
    }
  });

  const romInstructions = {
    "roms/ibm.ch8": {
      desc: "IBM Test Screen",
      keys: "No controls - Graphical benchmark."
    },
    "roms/breakout.ch8": {
      desc: "Breakout",
      keys: "[Q] Left | [E] Right"
    },
    "roms/flight-runner.ch8": {
      desc: "Flight Runner",
      keys: "[W] Ascend | [S] Descend"
    },
    "roms/pong.ch8": {
      desc: "Pong (1 Player)",
      keys: "[1] Up | [Q] Down"
    },
    "roms/tetris.ch8": {
      desc: "Tetris",
      keys: "[Q] Rotate | [W] Left | [E] Right | [A] Drop"
    }
  };

  const formatKeyHints = (text) => {
    return text.replace(/\[(.*?)\]/g, '<span class="key-cap">[$1]</span>');
  };

  if (romToLoad === "upload.ch8") {
    descEl.innerHTML = "<strong>CUSTOM ROM LOADED</strong>";
    keyEl.innerHTML = formatKeyHints("Layout: [1][2][3][4] | [Q][W][E][R] | [A][S][D][F] | [Z][X][C][V]");
  } else if (romInstructions[romToLoad]) {
    descEl.innerHTML = `<strong>GAME:</strong> ${romInstructions[romToLoad].desc}`;
    keyEl.innerHTML = formatKeyHints(romInstructions[romInstructions[romToLoad] ? romToLoad : ""].keys);
  } else {
    if (panel) panel.style.display = 'none';
  }

  window.addEventListener('mousedown', () => {
    document.getElementById('canvas')?.focus();
  });
});
