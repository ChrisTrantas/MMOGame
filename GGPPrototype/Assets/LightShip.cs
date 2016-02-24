using UnityEngine;
using System.Collections;

public class LightShip : MonoBehaviour {

    Rigidbody2D myRigidbody;

    void Start()
    {
        myRigidbody = this.GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        myRigidbody.AddForce(new Vector2(Input.GetAxis("HorizontalMove") * 5, Input.GetAxis("VerticalMove") * 5));

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
