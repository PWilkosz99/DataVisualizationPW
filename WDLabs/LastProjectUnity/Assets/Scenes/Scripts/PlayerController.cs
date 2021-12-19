using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public CharacterController characterController;

    public float moveSpeed = 9.0f;
    public float jumpHeight = 7.0f;
    public float actualJumpHeight = 0.0f;

    private void keyboardAction()
    {
        float moveFrontBack = Input.GetAxis("Vertical") * moveSpeed;
        float moveLeftRight = Input.GetAxis("Horizontal") * moveSpeed;

        if (characterController.isGrounded && Input.GetButton("Jump"))
        {
            actualJumpHeight = jumpHeight;
        }
        else if (!characterController.isGrounded) //in air
        {
            actualJumpHeight += Physics.gravity.y * Time.deltaTime;
        }

        Vector3 moveVector = new Vector3(moveLeftRight, actualJumpHeight, moveFrontBack);
        moveVector = transform.rotation * moveVector;

        characterController.Move(moveVector * Time.deltaTime);
    }

    // Start is called before the first frame update
    void Start()
    {
        characterController = GetComponent<CharacterController>();
    }

    // Update is called once per frame
    void Update()
    {
        keyboardAction();
    }
}
