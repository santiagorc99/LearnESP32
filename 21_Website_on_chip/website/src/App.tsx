import { useState, useEffect } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'

function App() {
  const [message,setMessage] = useState("");
  const [led_on,setLedOn] = useState(false);
  const [button_state, setBtnState] = useState(false);

  // Using [] renders the function only once.
  useEffect(()=>{
    get_hello_world();
    web_socket();
  }, []);

  const get_hello_world = async () => {
    const web_result = await fetch("/api/hello-world");
    const my_text = await web_result.text();
    setMessage(my_text);
  };

  const switch_led = async (is_on: boolean) => {
    const payload = { is_on };
    const webResult = await fetch("/api/toggle-led", {
      method: "POST",
      body: JSON.stringify(payload),
    });
    if (!webResult.ok) {
      console.error(webResult.statusText);
      return;
    }
    setLedOn(is_on);
  };

  const get_led_text = () => led_on? "LED is ON" : "LED is OFF"

  const get_web_socket_url = (suffix: string) => {
    const l = window.location;
    return ((l.protocol == "https:")? "wss://" : "ws://") + l.host + l.pathname + suffix;
  }

  const web_socket = () => {
    const socket = new WebSocket(get_web_socket_url("ws"));
    socket.onopen = () => {
      socket.send("Hello ESP32");
    }
    socket.onmessage = (event) => {
      console.log(event.data);
      try {
        const attempted_button_state = JSON.parse(event.data);
        setBtnState(attempted_button_state.button_state);
      } finally {
        return;
      }
    }
    socket.onerror = (err) => console.log(err)
    socket.onclose = (event) => console.log(event)
  }

  return (
    <>
      <div>
        <a href="https://vitejs.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>
        {message}
      </h1>
      <div className="card">
        <button
          style={{backgroundColor: led_on? "lightblue" : ""}}
          onClick={() => switch_led(!led_on)}
        >
          {get_led_text()}
        </button>
        <div
          style={{
            width: "50px", 
            height: "50px", 
            border: "1px solid black", 
            margin:"auto",
            borderRadius: "50px",
            backgroundColor: button_state? "pink" : "",
          }}
        >
          
        </div>
        <p>
          Edit <code>src/App.tsx</code> and save to test HMR
        </p>
      </div>
      <p className="read-the-docs">
        Click on the Vite and React logos to learn more
      </p>
    </>
  )
}

export default App
