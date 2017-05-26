class RemoteSwitch {
  constructor(onStateChangeCallback, onTimeChangeCallback) {
    this._state = false;
    this._time = "00:00";
    this.onStateChangeCallback = onStateChangeCallback;
    this.onTimeChangeCallback = onTimeChangeCallback;
    this._d("constructor()", "done");
  }

  connectRequest() {
    webusb.requestPort().then(selectedPort => {
      this._connect(selectedPort);
    }).catch(error => {
      console.error('Connection error (1): ' + error);
      this._state = !this._state; // revert state back
      this.onStateChangeCallback(this.state);
    });
  }

  _connect(port) {
    this._port = port;
    this._d('device.productName', this._port.device_.productName);
    this._port.connect().then(() => {
      this._d('port', this._port);
      this._port.onReceive = data => {
        this._proceed(data);
      };
      this._port.onReceiveError = error => {
        console.log('Receive error: ' + error);
      };
      this._sendState(); // set current state
    }, error => {
      console.log('Connection error (2): ' + error);
      this._state = !this._state; // revert state back
      this.onStateChangeCallback(this.state);
    });
  }

  get state() {
    return this._state;
  }
  set state(state) {
    this._d("state", state);
    if (this._state == state) { // state already set, don't send anything
      return;
    }
    
    this._state = state;
    if (this._port == undefined) {
      this.connectRequest();
    } else {
      this._sendState();
    }
  }

  get time() {
    if (this._port == undefined || !this._state) {
      return "00:00";
    }
    return this._time;
  }
  // set time(time) {
  //   this._time = time;
  //   this._d("time", this.time);
  // }
  
  _proceed(value) {
    this._d('data', value);
    if (value.startsWith("LOW")) {
      this._state = false;
      this._d("state", this.state);
      this.onStateChangeCallback(this.state);
    } else if (value.startsWith("HIGH")) {
      this._state = true;
      this._d("state", this.state);
      this.onStateChangeCallback(this.state);
    } else if (value.startsWith("T")) {
      this._time = value.substring(1);
      this._d("time", this._time);
      this.onTimeChangeCallback(this.time);
    }
  }

  _sendState() {
    this._port.send(this._state ? "H" : "L");
  }

  _d(what, value) {
    console.log(what + "=" + value);
  }
}

