using UnityEngine;
using System.Collections;

public class LightInput : MonoBehaviour {

    float lightAngle;
    float horz = 0;
    float vert = 0;

	// Update is called once per frame
	void Update () {
        horz = Input.GetAxis("Horizontal");
        vert = Input.GetAxis("Vertical");
        if (horz != 0 || vert != 0)
        {
            this.transform.rotation = Quaternion.AngleAxis(-Mathf.Atan2(horz, vert) * Mathf.Rad2Deg + 90, new Vector3(0, 0, 1));
        }

	}
}
