const CryptoJS = require('crypto-js') // Standard JavaScript cryptography library
const fetch = require('node-fetch') // "Fetch" HTTP req library
const apiKey = '' // const apiKey = 'paste key here'
const apiSecret = '' // const apiSecret = 'paste secret here'
const apiPath = 'api/v1/fire'// Example path
const nonce = (Date.now() * 1000).toString() // Standard nonce generator. Timestamp * 1000
const body = {'alarm' : true} // Field you may change depending on endpoint
let signature = `${apiPath}${nonce}${JSON.stringify(body)}`
// Consists of the complete url, nonce, and request body
const sig = CryptoJS.HmacSHA1(signature, apiSecret).toString()
// The authentication signature is hashed using the private key
fetch(`http://192.168.1.100:8080/${apiPath}`, {
method: 'POST',
body: JSON.stringify(body),
headers: {
'Content-Type': 'application/json',
'trombon-nonce': nonce,
'trombon-apikey': apiKey,
'trombon-signature': sig
}
})
.then(res => res.json())
.then(json => console.log(json)) //Logs the response body
.catch(err => {
console.log(err)
})