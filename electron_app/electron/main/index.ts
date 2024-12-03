import { app, BrowserWindow, shell, ipcMain } from 'electron'
import { createRequire } from 'node:module'
import { fileURLToPath } from 'node:url'
import path from 'node:path'
import os from 'node:os'
import jetpack from 'fs-jetpack'
import { SerialPort } from 'serialport'
import { Buffer } from 'node:buffer';

const require = createRequire(import.meta.url)
const __dirname = path.dirname(fileURLToPath(import.meta.url))

// The built directory structure
//
// ├─┬ dist-electron
// │ ├─┬ main
// │ │ └── index.js    > Electron-Main
// │ └─┬ preload
// │   └── index.mjs   > Preload-Scripts
// ├─┬ dist
// │ └── index.html    > Electron-Renderer
//
process.env.APP_ROOT = path.join(__dirname, '../..')

export const MAIN_DIST = path.join(process.env.APP_ROOT, 'dist-electron')
export const RENDERER_DIST = path.join(process.env.APP_ROOT, 'dist')
export const VITE_DEV_SERVER_URL = process.env.VITE_DEV_SERVER_URL

process.env.VITE_PUBLIC = VITE_DEV_SERVER_URL
  ? path.join(process.env.APP_ROOT, 'public')
  : RENDERER_DIST

// Disable GPU Acceleration for Windows 7
if (os.release().startsWith('6.1')) app.disableHardwareAcceleration()

// Set application name for Windows 10+ notifications
if (process.platform === 'win32') app.setAppUserModelId(app.getName())

if (!app.requestSingleInstanceLock()) {
  app.quit()
  process.exit(0)
}

/* -------------------
   Schema send/receive
   ------------------- */
if (process.env.NODE_ENV !== 'development') {
  global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
} else {
  global.__static = './static';
}

var device_schema = null;

ipcMain.handle('get_schema', async (event) => {
  return jetpack.read(path.join(__static, './config.json'), 'json');
});

function bytecount(definition) {
  const bytecount_map = {
    uint8_t:  [1, false],
    int8_t:   [1, true],
    uint16_t: [2, false],
    int16_t:  [2, true],
    uint32_t: [4, false],
    int32_t:  [4, true],
    uint64_t: [8, false],
    int64_t:  [8, true],
    char:     [1, false],
    float:    [4, true],
    double:   [8, true],
    int:      [4, true],
  };
  return bytecount_map[definition.type];
}

function binary_to_int_string(buf, signed) {
  buf = buf.reverse();
  var num = BigInt(0);
  for (let i = 0; i < buf.length; i++) {
    num <<= BigInt(8);
    num |= BigInt(buf[i]);
  }

  num = signed ? BigInt.asIntN(buf.length * 8, num) : BigInt.asUintN(buf.length * 8, num);
  return num.toString();
}

function binary_to_char_number(buf) {
  console.assert(buf.length == 1)
  return buf[0]
}

function int_string_to_binary(value, bytecount, signed) {
  const result = [];

  const value_bi = BigInt(value);
  var num = signed ? BigInt.asIntN(bytecount * 8, value_bi) : BigInt.asUintN(bytecount * 8, value_bi);
  for (let i = 0; i < bytecount; i++) {
    result.push(Number(num & BigInt(0xFF)));
    num >>= BigInt(8);
  }

  return result;
}

function FloatToIEEE(f) {
  var buf = new ArrayBuffer(4);
  (new Float32Array(buf))[0] = f;
  return new Uint8Array(buf);
}

function IEEEtoFloat(buf) {
  return (new Float32Array(Uint8Array.from(buf).buffer))[0];
}

function DoubleToIEEE(d) {
  var buf = new ArrayBuffer(8);
  (new Float64Array(buf))[0] = d;
  return new Uint8Array(buf);
}

function IEEEtoDouble(buf) {
  return (new Float64Array(Uint8Array.from(buf).buffer))[0];
}

function write_definition(definition) {
  if (definition.type == "float" || definition.type == "double") {
    var [bytes, signed] = bytecount(definition);
    bytes = definition._bytec || bytes;

    if (bytes == 4) {
      return FloatToIEEE(definition.value);
    } else if (bytes == 8) {
      return DoubleToIEEE(definition.value);
    } else {
      console.log("WARNING: unsure what to do with: ", definition);
      return undefined;
    }
  } else {
    var [bytes, signed] = bytecount(definition);
    bytes ||= definition._bytec;
    return int_string_to_binary(definition.value, bytes, signed);
  }
}

function read_definition(definition, value) {
  if (definition.type == "float" || definition.type == "double") {
    if (value.length == 4) {
      return IEEEtoFloat(value);
    } else if (value.length == 8) {
      return IEEEtoDouble(value);
    } else {
      console.log("WARNING: unsure what to do with: ", value, accessor)
    }
  } else if (definition.type == "char") {
    return binary_to_char_number(value);
  } else {
    return binary_to_int_string(value);
  }
}

function find_diff(orig_schema, new_schema, changes, accessor, matchall) {
  /* original schema, new schema -> [[[accessor, path, ...], binary value], ...]

  Recursively compares the new schema against the old, adding changes as found.
  In some situations, such as the new schema having a longer list of a
  collection type, there isn't a branch to compare; matchall is set */
  if (new_schema.kind == "struct") {
    for (let [index, field] of new_schema.fields.entries()) {
      if (matchall || field.value != orig_schema.fields[index].value) {
        changes.push([[...accessor].concat([field._accessor]),
                     write_definition(field)])
      }
    }
    for (let [index, child] of new_schema.children.entries()) {
      var child_schema = null;
      if (orig_schema) {
        child_schema = orig_schema.children[index]
      }
      find_diff(child_schema, child, changes,
                [...accessor].concat([child._accessor]),
                matchall);
    }
  } else if (new_schema.kind == "list"){
    // list
    if (matchall || new_schema.items.length != orig_schema.items.length) {
      // list length has changed
      changes.push([[...accessor].concat([1]),
                    [new_schema.items.length & 0xFF, (new_schema.items.length >> 8) & 0xFF ]])
    }

    if (new_schema.type.kind == "primitive") {
      function listvalornull(l, i) {
        const unmatchable = Object();
        return i < l.length ? l[i].value : unmatchable;
      }
      for (let [index, elem] of new_schema.items.entries()) {
        if (matchall || elem.value != listvalornull(orig_schema.items, index)) {
          changes.push([[...accessor].concat([index + 3]),
                        write_definition(elem)])
        }
      }
    } else {
      for (let [index, elem] of new_schema.items.entries()) {
        if (matchall || index < orig_schema.items.length) {
          var child_schema = null;
          if (orig_schema) {
            child_schema = orig_schema.items[index];
          }
          find_diff(child_schema, elem, changes,
                    [...accessor].concat([index + 3]),
                    matchall);
        } else {
          // turn on matchall -- there is no matching sub-collection at all
          var child_schema = null;
          if (orig_schema) {
            child_schema = orig_schema.type;
          }
          find_diff(child_schema, elem, changes,
                    [...accessor].concat([index + 3]),
                    true);
        }
      }
    }
  }
  return changes
}

ipcMain.handle('serial_paths', async (event) => {
  console.log('Scanning for open ports');
  return new Promise(async (resolve, reject) => {
    await SerialPort.list().then((ports, err) => {
      if (ports.length) {
        resolve(ports);
      } else if (err) {
        reject(err);
      }
    })
  });
});

ipcMain.handle('update_schema', async (event, schema, path) => {
  console.log('Syncing schema update:', schema, path);

  // compare the new and old schemas, deriving changes
  const matchall = (device_schema == null);
  const changes = find_diff(device_schema, schema, [], [], matchall);
  console.log("found changes:", changes);

  const protocol = new SerialProtocol(path)
  return protocol.write_updates(changes);
});

function next_read_accessor(protocol, read_queue, schema, resolve_fn, reject_fn) {
  /* Recursively process the read results into a new read queue */
  if (read_queue.length == 0) {
    device_schema = schema;
    resolve_fn(schema);
    return;
  }

  protocol.read_accessors(read_queue).then(
    (read_results) => {
    const next_queue = [];
    for (let [accessor, callback_fn, value] of read_results) {
      callback_fn(value, next_queue);
    }
    next_read_accessor(protocol, next_queue, schema, resolve_fn, reject_fn);
  },
    (read_err) => {
      reject_fn(read_err);
    }
  );
}

function find_read_accessors(definition, read_queue, accessor_ctx) {
  /* Return the next set of reads needed for this schema, as a list of
     (accessor, setter callback) entries.

     The setter callback is called with the value and next read queue; it sets
     the value on the schema entry, and puts the next set of reads onto the
     next read queue. (This is necessary for schema types like the list, where
     the current list size is needed first to ask for the complete set of list
     values.)
   */
  if (definition.kind == "struct") {
    // ask about primitives, then descend into collections
    for (let field of definition.fields) {
      const accessor = [...accessor_ctx].concat([field._accessor])
      read_queue.push([accessor, function(value, _) {
        field.value = read_definition(field, value)
        field._bytec = value.length
      }]);
    }

    for (let child of definition.children) {
      find_read_accessors(child, read_queue, [...accessor_ctx].concat([child._accessor]));
    }
  } else if (definition.kind == "list") {
    // ask about list size (member index 1), then ask about list elements
    const accessor = [...accessor_ctx].concat([1]);
    read_queue.push([accessor, function(value_buffer, nread_queue) {
      const value = binary_to_int_string(value_buffer, false);
      definition.items = definition.items.slice(0, value);

      const addc = value - definition.items.length;
      for (let i = 0; i < addc; i++) {
        definition.items.push(JSON.parse(JSON.stringify(definition.type)))
      }

      for (let [index, child_def] of definition.items.entries()) {
        if (definition.type.kind == "primitive") {
          const subaccessor = [...accessor_ctx].concat([index + 3]);
          nread_queue.push([subaccessor, function(value, _) {
            child_def.value = read_definition(value);
            child_def._bytec = value.length
          }]);
        } else {
          find_read_accessors(child_def, nread_queue, [...accessor_ctx].concat([index + 3]));
        }
      }
    }]);
  }
  return read_queue;
}

ipcMain.handle('read_device_config', async (event, schema, path) => {
  console.log('Reading the device configuration:', path);

  const protocol = new SerialProtocol(path);
  return new Promise((resolve, reject) => {
    next_read_accessor(protocol, find_read_accessors(schema, [], []), schema, resolve, reject);
  });
});

const [SC_PROTO_OPEN,
       SC_PROTO_MARCO,
       SC_PROTO_TRX_FETCH,
       SC_PROTO_READ,
       SC_PROTO_READ_HOLD,
       SC_PROTO_UPDATE,
       SC_PROTO_COMMIT,
       SC_PROTO_SYNC_NEEDED,
       SC_PROTO_ERR] = [1, 2, 3, 4, 5, 6, 7, 8, 9];

const [SC_OPCODE_READ,
       SC_OPCODE_WRITE,
       SC_OPCODE_DESCRIBE,
       SC_OPCODE_MARCO,
       SC_OPCODE_OPEN_TRX,
       SC_OPCODE_HOLD,
       SC_OPCODE_COMMIT_TRX,
       SC_OPCODE_CANCEL_TRX,
       SC_OPCODE_TRX_ID,
       SC_OPCODE_NEED_SYNC,
       SC_OPCODE_DEBUG] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12];

const [SC_STATUS_OK,
       SC_STATUS_OUT_OF_BOUNDS,
       SC_STATUS_WRITE_PROTECTED,
       SC_STATUS_INVALID_ACCESSOR,
       SC_STATUS_INVALID_TYPE,
       SC_STATUS_BROKEN_PTR,
       SC_STATUS_INVALID_LENGTH,
       SC_STATUS_LENGTH_OVERFLOW,
       SC_STATUS_LENGTH_MISMATCH,
       SC_STATUS_INVALID_OPCODE,
       SC_STATUS_NEED_SYNC,
       SC_STATUS_NO_TRX,
       SC_STATUS_TIMEOUT] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13];

class SerialProtocol {
  protocol_state = SC_PROTO_OPEN;
  read_buf = []; // currrent buffer; accumulates read packets into commands
  challenge_buf = []; // random handshake packet

  cmd_id = 0;
  cmd_watchdog_id = -1;
  update_watchdog_id = -1;

  mode = -1; // mode, set after marco-polo success
  promise_resolve = null; // success callback (update/read operation)
  promise_reject = null;  // failure callback

  changes = []; // queue of writes, in update mode
  trx_id = -1;  // transaction id sent during commit, in update mode

  reads = [];   // queue of reads, in read mode
  read_results = []; // results of reads, in read mode

  constructor(device_path) {
    this.port = new SerialPort({ path: device_path,
                                 baudRate: 9600,
                                 lock: false,
                                 autoOpen: false});

    this.port.on('error', this.handle_error(this));
    this.port.on('open', this.handle_open(this));
    this.port.on('data', this.handle_read(this));
    this.command_watchdog(this);
  }

  command_watchdog(sp) {
    sp.cmd_watchdog_id = setInterval(function() {
      const last_cmd_id = sp.cmd_id;
      console.log("Watchdog running for cmd:", last_cmd_id);
      setTimeout(function() {
        if (last_cmd_id == sp.cmd_id) {
          console.log("Watchdog set off")
          // no progress...
          sp.handle_error(sp)(new Error("Timout passed without command progress..."));
        }
      }, 5000);
    }, 5000);
  }

  interaction_watchdog(sp) {
    cp.update_watchdog_id = setTimeout(function() {
      sp.handle_error(sp)(new Error("Timeout passed without interaction complete..."));
    }, 20000);
  }

  cleanup() {
    clearInterval(this.cmd_watchdog_id);
    clearTimeout(this.update_watchdog_id);
    this.close();
  }

  write_updates(changes) {
    this.changes = []
    this.changes.push(...changes);

    this.mode = SC_PROTO_UPDATE;

    return new Promise((resolve, reject) => {
      this.promise_resolve = resolve;
      this.promise_reject = reject;

      if (this.port.isOpen) {
        this.handle_open(this)();
      } else {
        this.port.open();
      }
    });
  }

  read_accessors(reads) {
    this.reads = [];
    this.read_results = [];
    this.reads.push(...reads);

    const promise = new Promise((resolve, reject) => {
      this.promise_resolve = resolve;
      this.promise_reject = reject;

      this.mode = SC_PROTO_READ;
      if (this.port.isOpen) {
        this.handle_open(this)();
      } else {
        this.port.open();
      }
    });

    return promise;
  }

  handle_error(sp) {
    return function(err) {
      sp.cleanup();
      sp.promise_reject(err)
    }
  }

  handle_open(sp) {
    return function() {
      sp.protocol_state = SC_PROTO_MARCO;
      sp.write_marco();
    }
  }

  close() {
    if (this.port.isOpen) {
      this.port.close();
    }
  }

  handle_read(obj) {
    return function(data) {
      console.log('Read data:', data);
      obj.read_buf.push(...data);
      console.log('Buffer: (', obj.read_buf.length, ")", obj.read_buf);

      // minimal packet is 4 bytes: opcode | resultcode | datalen | data ... | 0x0A (\n)
      if (obj.read_buf.length < 4) {
        return
      }

      const [opcode, result_code, datalen] = [obj.read_buf[0], obj.read_buf[1], obj.read_buf[2]];
      if (obj.read_buf.length < 4 + datalen) {
        return
      }
      const data_buf = obj.read_buf.slice(3, 3 + datalen);

      if (opcode == SC_OPCODE_DEBUG) {
        const buf = Buffer.from(data_buf, "hex");
        console.log("DEVICE DEBUG:", buf.toString("ascii"));
        obj._reposition_read_buffer(3 + datalen);
        obj.handle_read(obj)([]);
      } else if (obj.protocol_state == SC_PROTO_MARCO) {
        console.log("marco state")
        if (opcode == SC_OPCODE_MARCO && result_code == SC_STATUS_OK && obj._compare_challenge_buffer(data_buf)) {
          console.log("marco buffer successfully received: ", datalen, ", ", data_buf);
          obj.protocol_state = SC_PROTO_TRX_FETCH;
          obj._reposition_read_buffer(3 + datalen)
          obj.write_trx_fetch();
        } else {
          console.log("marco command failed");
          console.log("opcode: ", opcode, "result code:", result_code);
          obj.protocol_state = SC_PROTO_SYNC_NEEDED;
          obj.write_sync();
          return;
        }
      } else if (obj.protocol_state == SC_PROTO_TRX_FETCH) {
        console.log("trx fetch")
        if (opcode == SC_OPCODE_TRX_ID && result_code == SC_STATUS_OK && datalen == 1) {
          obj.protocol_state = obj.mode;
          obj.trx_id = data_buf[0];
          obj.cmd_id++;
          obj._reposition_read_buffer(3 + datalen);
          console.log("Found trx id:", obj.trx_id);
          if (obj.protocol_state == SC_PROTO_UPDATE) {
            obj.write_change();
          } else {
            obj.write_read_accessor();
          }
        } else {
          console.log("trx fetch broken -- sync needed");
          console.log("opcode: ", opcode, "result code:", result_code);
          obj.protocol_state = SC_PROTO_SYNC_NEEDED;
          obj.write_sync();
          return;
        }
      } else if (obj.protocol_state == SC_PROTO_UPDATE) {
        console.log("protocol update")
        if (opcode == SC_OPCODE_WRITE && result_code == SC_STATUS_OK) {
          console.log("successfully completed write...");
          obj._reposition_read_buffer(3 + datalen);
          obj.cmd_id++;
          obj.changes.shift();
          if (obj.changes.length) {
            obj.write_change();
          } else {
            obj.protocol_state = SC_PROTO_COMMIT;
            obj.write_trx_commit();
          }
        } else {
          console.log("Write broken -- sync needed");
          console.log("opcode: ", opcode, "result code:", result_code);
          obj.protocol_state = SC_PROTO_SYNC_NEEDED;
          obj.write_sync();
          return;
        }
      } else if (obj.protocol_state == SC_PROTO_COMMIT) {
        console.log("commit packet");
        const device_trx = data_buf[0];
        const expected_trx = (obj.trx_id + 1) % 256;
        if (opcode == SC_OPCODE_COMMIT_TRX && result_code == SC_STATUS_OK && datalen == 1 && device_trx == expected_trx) {
          console.log("Transaction successful");
          obj.cmd_id++;
          obj._reposition_read_buffer(3 + datalen);
          obj.cleanup();
          obj.promise_resolve("Update successful");
        } else {
          console.log("Error getting commit back -- sync needed");
          console.log("opcode: ", opcode, "result code:", result_code);
          obj.protocol_state = SC_PROTO_SYNC_NEEDED;
          obj.write_sync();
          return;
        }
      } else if (obj.protocol_state == SC_PROTO_READ) {
        console.log("proto read...")
        if (obj.read_buf.length < 3) {
          // wait for more data
          return;
        }
        if (opcode == SC_OPCODE_READ && result_code == SC_STATUS_OK) {
          const value_buf = data_buf;
          obj._reposition_read_buffer(3 + data_buf.length);
          obj.cmd_id++;
          obj.read_results.push(obj.reads.shift().concat([value_buf]));
          if (obj.reads.length) {
            obj.write_read_accessor();
          } else {
            console.log("Read success");
            obj.protocol_state = SC_PROTO_READ_HOLD;
            obj.write_hold();
          }
        } else {
          console.log("Error with read");
          console.log("opcode: ", opcode, "result code:", result_code);
          obj.protocol_state = SC_PROTO_SYNC_NEEDED;
          obj.write_sync();
          return;
        }
      } else if (obj.protocol_state == SC_PROTO_READ_HOLD) {
        console.log("read hold");
        if (opcode == SC_OPCODE_HOLD && result_code == SC_STATUS_OK) {
          console.log("Read hold successful...")
          obj._reposition_read_buffer(3 + datalen);
          obj.cleanup();
          obj.promise_resolve(obj.read_results);
          obj.cmd_id++;
        } else {
          obj.protocol_state = SC_PROTO_SYNC_NEEDED;
          obj.write_sync();
        }
      } else if (obj.protocol_state == SC_PROTO_SYNC_NEEDED) {
        while (true) {
          // scan to 0 segment
          while (obj.read_buf.length > 0 && obj.read_buf[0] != 0) {
            obj.read_buf.shift();
          }

          if (obj.read_buf.length <= 600) {
            // wait for more data
            return;
          }

          // try to count 600 0s
          var i;
          for (i = 0; i < 600; i++) {
            if (obj.read_buf[i] != 0) {
              obj.read_buf.splice(0, i + 1);
              break;
            }
          }

          if (obj.read_buf[i] == '\n')
            i++;

          obj._reposition_read_buffer(i);
          if (i >= 600) {
            // sync completed
            obj.protocol_state = SC_PROTO_MARCO;
            obj.write_marco();
            return;
          }
        }
      } else {
        console.log("no known handler for state", obj.protocol_state)
      }
    }
  }

  _reposition_read_buffer(start) {
    this.read_buf.splice(0, start);
    while (this.read_buf.length && this.read_buf.shift() != 0x0A)
      ;
  }

  _compare_challenge_buffer(read_buffer) {
    if (read_buffer.length != this.challenge_buf.length)
      return false;

    for (let i = 0; i < read_buffer.length; i++)
      if (read_buffer[i] != this.challenge_buf[i])
        return false;

    return true;
  }

  write_marco() {
    console.log("Writing marco packet");
    const buf = [SC_OPCODE_MARCO, 0x08, 0x00];

    this.challenge_buf = [];
    for (let i = 0; i < 8; i++) {
      this.challenge_buf.push(this._rand_int(256));
    }
    buf.push(...this.challenge_buf);

    buf.push(0x0A);
    this.port.write(buf, "hex");
  }

  write_trx_commit() {
    console.log("Finished update; sending trx commit");
    const buf = [SC_OPCODE_COMMIT_TRX, 0x01, 0x00, this.trx_id, 0x0A];
    this.port.write(buf, "hex");
  }

  _rand_int(max) {
    return Math.floor(Math.random() * max);
  }

  write_trx_fetch() {
    const buf = [SC_OPCODE_TRX_ID, 0x00, 0x00, 0x0A];
    console.log("Writing trx id fetch");
    this.port.write(buf, "hex");
  }

  write_read_accessor() {
    console.log("writing read accessor");
    if (!this.reads.length) {
      return;
    }

    const [accessor, token] = this.reads[0];
    const buf = [SC_OPCODE_READ, accessor.length, 0]
    buf.push(...accessor)
    buf.push(0x0A)

    this.port.write(buf, "hex")
  }

  write_hold() {
    console.log("Writing hold");
    const buf = [SC_OPCODE_HOLD, 1, 0, 1, 0x0A];
    this.port.write(buf, "hex");
  }

  write_change() {
    console.log("Writing update");
    if (!this.changes.length) {
      return;
    }

    const [accessor, value] = this.changes[0];

    const buf = [SC_OPCODE_WRITE, accessor.length, value.length];
    buf.push(...accessor);
    buf.push(...value);

    // newline
    buf.push(0x0A);
    this.port.write(buf, "hex");
  }

  write_sync() {
    console.log("Writing sync packet");

    const buf = [SC_OPCODE_NEED_SYNC, 0x00, 0x00, 0x0A]
    for (let i = 0; i < 1200; i++) {
      buf.push(0x00);
    }

    buf.push(0x0A);
    this.port.write(buf, "hex");
  }
}

let win: BrowserWindow | null = null
const preload = path.join(__dirname, '../preload/index.mjs')
const indexHtml = path.join(RENDERER_DIST, 'index.html')

async function createWindow() {
  win = new BrowserWindow({
    title: 'Main window',
    icon: path.join(process.env.VITE_PUBLIC, 'favicon.ico'),
    webPreferences: {
      preload,
      // Warning: Enable nodeIntegration and disable contextIsolation is not secure in production
      nodeIntegration: true,

      // Consider using contextBridge.exposeInMainWorld
      // Read more on https://www.electronjs.org/docs/latest/tutorial/context-isolation
      // contextIsolation: false,
    },
  })

  if (VITE_DEV_SERVER_URL) { // #298
    win.loadURL(VITE_DEV_SERVER_URL)
    // Open devTool if the app is not packaged
    win.webContents.openDevTools()
  } else {
    win.loadFile(indexHtml)
  }

  // Test actively push message to the Electron-Renderer
  win.webContents.on('did-finish-load', () => {
    win?.webContents.send('main-process-message', new Date().toLocaleString())
  })

  // Make all links open with the browser, not with the application
  win.webContents.setWindowOpenHandler(({ url }) => {
    if (url.startsWith('https:')) shell.openExternal(url)
    return { action: 'deny' }
  })
  // win.webContents.on('will-navigate', (event, url) => { }) #344
}

app.whenReady().then(createWindow)

app.on('window-all-closed', () => {
  win = null
  if (process.platform !== 'darwin') app.quit()
})

app.on('second-instance', () => {
  if (win) {
    // Focus on the main window if the user tried to open another
    if (win.isMinimized()) win.restore()
    win.focus()
  }
})

app.on('activate', () => {
  const allWindows = BrowserWindow.getAllWindows()
  if (allWindows.length) {
    allWindows[0].focus()
  } else {
    createWindow()
  }
})

// New window example arg: new windows url
ipcMain.handle('open-win', (_, arg) => {
  const childWindow = new BrowserWindow({
    webPreferences: {
      preload,
      nodeIntegration: true,
      contextIsolation: false,
    },
  })

  if (VITE_DEV_SERVER_URL) {
    childWindow.loadURL(`${VITE_DEV_SERVER_URL}#${arg}`)
  } else {
    childWindow.loadFile(indexHtml, { hash: arg })
  }
})



