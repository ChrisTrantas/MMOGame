using UnityEngine;
using System.Collections;

public class Asteroid : MonoBehaviour {

    float angDir;
    Rigidbody2D myRigidbody;

	// Use this for initialization
	void Start ()
    {
        angDir = Random.Range(-1, 1);
        GetComponent<Rigidbody2D>().velocity = Random.onUnitSphere;
        myRigidbody = GetComponent<Rigidbody2D>();
	}
	
	// Update is called once per frame
	void Update ()
    {
        transform.Rotate(0, angDir * Mathf.PI / (GetComponent<CircleCollider2D>().radius * transform.localScale.x * 3), 0);

        if (transform.position.x < -8.58f)
        {
            myRigidbody.MovePosition(new Vector2(transform.position.x + 2 * 8.58f, transform.position.y));

        }
        else if (transform.position.x > 8.58f)
        {
            myRigidbody.MovePosition(new Vector2(transform.position.x - 2 * 8.58f, transform.position.y));
        }

        if (transform.position.y < -5)
        {
            myRigidbody.MovePosition(new Vector2(transform.position.x, transform.position.y + 10));
        }
        else if (transform.position.y > 5)
        {
            myRigidbody.MovePosition(new Vector2(transform.position.x, transform.position.y - 10));
        }
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.tag == "bullet")
        {
            Destroy(other.gameObject);
            Explode();
        }
    }

    void Explode()
    {
        Debug.Log("Exploding");
        if (transform.localScale.x >= 0.4) //if the asteroid is not too small to split up
        {
            Vector3 spawnDir = transform.forward;
            spawnDir.Normalize();
            for (int i = 0; i < 3; i++) //spawn 3 new asteroids
            {
                GameObject ast = (GameObject)Instantiate(gameObject, transform.position, Quaternion.AngleAxis(90, Vector3.left) * Quaternion.identity);
                ast.transform.localScale = ast.transform.localScale - new Vector3(0.3f, 0.3f, 0.3f); //with a smaller scale
                ast.transform.position = ast.transform.position + spawnDir * (float)(GetComponent<CircleCollider2D>().radius * (ast.transform.localScale.x / 2.5)); //positioned radially
                ast.transform.GetComponent<Rigidbody2D>().velocity = spawnDir * (float)(1 + (1.0f - ast.transform.localScale.x) * 2.5); //and set to move out radially
                spawnDir = Quaternion.AngleAxis(Mathf.PI * 2 / 3, Vector3.up) * spawnDir;
            }
        }
        Destroy(gameObject);
    }
}
