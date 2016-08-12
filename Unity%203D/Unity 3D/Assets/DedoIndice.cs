using UnityEngine;
using System.Collections;

public class DedoIndice : MonoBehaviour {

	float z;
	float posz = 0;
	
	// Use this for initialization
	void Start ()
	{
		
	}
	
	// Update is called once per frame
	void Update ()
	{
		z = ScriptQueLeeSerial.flex1;

		//Debug.Log(z.ToString());
		
		transform.eulerAngles = new Vector3(
			transform.eulerAngles.x,
			transform.eulerAngles.y,
			transform.eulerAngles.z + (z - posz)
		);

		posz = z;
	}
}
