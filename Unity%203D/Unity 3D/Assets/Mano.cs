using UnityEngine;
using System.Collections;
using System.IO.Ports;

public class Mano : MonoBehaviour {

	// Use this for initialization
	void Start () {
	}
	
	// Update is called once per frame
	void Update () {
		try {
			this.transform.rotation = new Quaternion( ScriptQueLeeSerial.x, ScriptQueLeeSerial.y, ScriptQueLeeSerial.z, ScriptQueLeeSerial.w);
		} catch (System.Exception ex){
			Debug.Log("Error: Update "+ex);
		}
	}
}
