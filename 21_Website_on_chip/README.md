## Run vite
npm create vite@latest
cd site
npm install
npm run dev

Main code is going to be installed at App.tsx

## Build my website
Go to packge.json inside the website page to check the options. In this case, we use npm run build to build the website.

Our site is going to be stored in website/dist/, it is to be loaded onto the chip. Some options are:

## Load the website
It's going to be placed into the flash using a FAT system.

## Server
We want our default uri to serve our website. We send the index.html file. However, it's not enough to get our website running since the html references other sources. To send all the files, we do not create a single endpoint for each file, instead, we create a wildcard for each file. We are going to modify the httpd_config_t uri_match_fn, it's a routine pointer. We are going to use it to create a function that will match what we have in the url to one of the uris we have defined ("config.uri_match_fn = httpd_uri_match_wildcard", it will allow us to use wildcards). To enable it, we are going to change the default uri from "/" to "/*" which will accept everything. 

The problem is, the rest of the uris will never be called. We solve it by changing the order of the uri definition, remeber: A request checks first the first defined uri. I.G. Move the default uri to the end, after the ws and api/toggle-led are defined.

## MIME Types
Now we are able to load a javascript file into the browser but we never told the browser it was a JS file (Same with all the rest of the files). To do so, we set the mime types: What kind of assets are being served.

Go to google and look for common mime types (For instance: .bmp is image/bmp, .css is text/css, .html is text/html and so on)

## Using REACT
We create a new endpoint just for testing purposes to hit our ESP website (/api/test). We modify the app.tsx file to update our website.