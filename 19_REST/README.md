# REST
* A chip that can access REST servicces can access the internet and get just about any data that you can think of or interact with any  service including
    - Getting stock quotes.
    - Send emails and SMS's.
    - Interact with social media (Twitter, Facebook, etc.).
    - Etc...

* Rest services are the most prolific way to get data and communicate to services over the internet.
* The acronym REST stands for Representational State Transfer, this basically means that each unique URL is a representation of some object.

## Trasnfer data
To transfer data we need one of the options below:
- URL: I  query params ?key=val&key2=val2.
- In header: Key vlaue pairs.
- Body: As a JSON.

## JSON (Java script object notation)
Used to transmit data objects consisting of key-value pairs. Example:

## REST API
* Communication -> Use client and server.
* "REST"ful web service -> A service that uses REST APIs to communicate.

REST APIs are:
* Simple and standardized.
* Scalable and stateless -> Allows us to easily make modification without caring about the state of the data.
* High performance as they support cache (Even though the server expands).

Example: we have a page and want to change some values remotely. 

First we connect our web page to a cloud based server via a REST API.
We have an endpoint, let's say: https://icecream.com/API/flavours.
* API is the API portion of the endpoint.
* Flavours is the resource.
The client sends a requests and the server responds to the client.

### Request
What actions or verbs can we use?
Action - HTTP method/operation
Create : Post
Read   : Get
Update : Put
Delete : Delete

In a request we have an operation, endpoint (REST API endpoint), parameters/body, and the headers (API key, authentication, etc...). We simply get a response.

### Example
1. We want to display what's currently in stock (flavours):
Request is: OP: Get, ENDPOINT: https://icecream.com/API/flavours.
Response is: A Json with all of the flavours.

2. A flavour (ID 1) run out of stock, we want to update it in the server to another flavor:
Request is: OP: put, EP: https://icecream.com/API/flavours/1 (1 is flavour id 1), body: data to be updated.
Response is: acknolwedge: Id 1 was updated to the new flavour.

3. A new flavour arrived, we want to load it into our website:
Request is: OP: Post, EP: https://icecream.com/API/flavours, body: flavour we want to create.
Response is: acknolwedge: Id 1 was updated to the new flavour.

## Chunking data
Create a buffer and a buffer index.
1. A new chunk has been passed to us.
2. It does not fit into our buffer so increase the memory size.
3. Move the data into memory at the position of the index offset.
4. Increase the index to the end of the data.
