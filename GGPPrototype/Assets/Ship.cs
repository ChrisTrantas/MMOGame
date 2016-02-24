using UnityEngine;
using System.Collections;

public class Ship : MonoBehaviour {

    Rigidbody2D myRigidbody;

    [SerializeField]
    GameObject bullet;
    [SerializeField]
    float speed;

    void Start()
    {
        myRigidbody = this.GetComponent<Rigidbody2D>();
    }
	
	// Update is called once per frame
	void Update () {
        if (Input.GetKey(KeyCode.A))
        {
            myRigidbody.AddForce(new Vector2(-5.0f, 0.0f));
        }
        if (Input.GetKey(KeyCode.D))
        {
            myRigidbody.AddForce(new Vector2(5.0f, 0.0f));
        }
        if (Input.GetKey(KeyCode.W))
        {
            myRigidbody.AddForce(new Vector2(0.0f, 5.0f));
        }
        if (Input.GetKey(KeyCode.S))
        {
            myRigidbody.AddForce(new Vector2(0.0f, -5.0f));
        }

        if (Input.GetKeyDown(KeyCode.Space))
        {
            GameObject newBullet = (GameObject)Instantiate(bullet, transform.position, Quaternion.identity);
            Physics2D.IgnoreCollision(newBullet.GetComponent<Collider2D>(), gameObject.GetComponent<Collider2D>());
            newBullet.GetComponent<Rigidbody2D>().velocity = myRigidbody.velocity.normalized * speed;
        }

        if (myRigidbody.velocity.sqrMagnitude > 25.0f)
        {
            myRigidbody.velocity = myRigidbody.velocity / myRigidbody.velocity.magnitude * 5;
        }

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

    void OnCollisionEnter2D(Collision2D other)
    {
        Debug.Log("dying");
        if (other.gameObject.tag == "asteroid")
        {
            Destroy(gameObject);
        }
    }
}
