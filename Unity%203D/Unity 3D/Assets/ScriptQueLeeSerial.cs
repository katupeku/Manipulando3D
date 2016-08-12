using UnityEngine;
using System.Collections;
using System.IO.Ports;

public class ScriptQueLeeSerial : MonoBehaviour {

	
	static public int flex0, flex1, flex2, flex3, flex4;
	static public float w, x, y, z;

	string serialValue;
	string[] vectorSerialValue; 
	SerialPort sp = new SerialPort("COM3",250000); //57600

	// Use this for initialization
	void Start () {
		try{
			sp.Open();
			//sp.ReadTimeout = 50;

			if (sp.IsOpen) {
				Debug.Log("Puerto abierto.");
			}
			else{
				Start();
			}
		} catch {
			Debug.Log("Error: No puede iniciar el puerto");
		}
	}
	
	// Update is called once per frame
	void Update () {
		try {
			if (sp.IsOpen) {
				serialValue = sp.ReadLine();
				vectorSerialValue = serialValue.Split(',');
				sp.BaseStream.Flush();

				if (vectorSerialValue.Length == 9) {

					flex0 = int.Parse(vectorSerialValue[0]);
					flex1 = int.Parse(vectorSerialValue[1]);
					flex2 = int.Parse(vectorSerialValue[2]);
					flex3 = int.Parse(vectorSerialValue[3]);
					flex4 = int.Parse(vectorSerialValue[4]);
					w     = float.Parse(vectorSerialValue[5]);
					z     = float.Parse(vectorSerialValue[6])*(-1);
					x     = float.Parse(vectorSerialValue[7]);
					y	  = float.Parse(vectorSerialValue[8])*(-1);

					//Debug.Log("X: "+x.ToString()+"     Y: "+y.ToString()+"     Z: "+z.ToString());
				}
			} else {
				Debug.Log("Error: el puerto esta cerrado");
				Start();
			}
		} catch (System.Exception ex){
			Debug.Log("Error: Update "+ex);
			sp.Close();
		}
	}
}
