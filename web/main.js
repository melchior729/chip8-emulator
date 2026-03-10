/**
 * @file Contains the Emscripten module config.
 * This script is the middleman between JavaScript & WebAssembly.
 * It manages ROM file uploading via the virtual filesystem.
 * @author Abhay Manoj
 */

const urlParams = new URLSearchParams(window.location.search);
const romToLoad = urlParams.get('rom') || "roms/ibm.ch8";
console.log(romToLoad);

var Module = {
  canvas: document.getElementById('canvas'),
  arguments: [romToLoad],
  print: (...args) => console.log(args.join(' ')),
  printErr: (...args) => console.error(args.join(' ')),
  setStatus: (text) => {
    if (text) {
      console.log("Emscripten: " + text);
    }
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
  if (romSelector) {
    romSelector.addEventListener('change', (e) => {
      window.location.href = `?rom=${encodeURIComponent(e.target.value)}`;
    });
  }

  const resetBtn = document.getElementById('btn-reset');
  if (resetBtn) {
    resetBtn.addEventListener('click', () => {
      if (romToLoad === "upload.ch8") {
        window.location.href = window.location.pathname;
      } else {
        window.location.reload();
      }
    });
  }

  const fileInput = document.getElementById('file-input');
  if (fileInput) {
    fileInput.addEventListener('change', function() {
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
  }

  const canvas = document.getElementById('canvas');
  if (canvas) {
    window.addEventListener('mousedown', () => {
      canvas.focus();
    });
  }

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

  const descEl = document.getElementById('game-desc');
  const keyEl = document.getElementById('key-map');
  const panel = document.getElementById('instructions-panel');

  if (romToLoad === "upload.ch8") {
    descEl.style.display = 'none';
    keyEl.innerHTML = "Custom ROM Loaded. Key Layout: [1234], [QWER], [ASDF], [ZXCV]";
  } else if (romInstructions[romToLoad]) {
    descEl.innerText = romInstructions[romToLoad].desc;
    keyEl.innerText = romInstructions[romToLoad].keys;
  } else {
    panel.style.display = 'none';
  }
});

