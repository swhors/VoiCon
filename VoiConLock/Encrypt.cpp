//---------------------------------------------------------------------------
//Parameters passed are:
//    szSource, the name of the input, a plaintext file.
//    szDestination, the name of the output, an encrypted file to be created.
//    szPassword, either NULL if a password is not to be used or the 
//                string that is the password.
static BOOL EncryptFile( PCHAR szSource, PCHAR szDestination,  PCHAR szPassword)
{ 
	//   Declare and initialize local variables.
	FILE *hSource; 
	FILE *hDestination; 

	HCRYPTPROV hCryptProv; 
	HCRYPTKEY hKey; 
	HCRYPTKEY hXchgKey; 
	HCRYPTHASH hHash; 

	PBYTE pbKeyBlob; 
	DWORD dwKeyBlobLen; 

	PBYTE pbBuffer; 
	DWORD dwBlockLen; 
	DWORD dwBufferLen; 
	DWORD dwCount; 

	//--------------------------------------------------------------------
	// Open source file. 

	if(hSource = fopen(szSource,"rb"))
	{
		printf("The source plaintext file, %s, is open. \n", szSource);
	}
	else
	{ 
		HandleError("Error opening source plaintext file!");
	} 
	//--------------------------------------------------------------------
	// Open destination file. 

	if(hDestination = fopen(szDestination,"wb"))
	{
		printf("Destination file %s is open. \n", szDestination);
	}
	else
	{
		HandleError("Error opening destination ciphertext file!"); 
	}
	// Get handle to the default provider. 
	if(CryptAcquireContext(
		&hCryptProv, 
		NULL, 
		MS_ENHANCED_PROV, 
		PROV_RSA_FULL, 
		0))
	{
		printf("A cryptographic provider has been acquired. \n");
	}
	else
	{
		HandleError("Error during CryptAcquireContext!"); 
	}
	//--------------------------------------------------------------------
	//   Create the session key.

	if(!szPassword ) 
	{ 
		//---------------------------------------------------------------
		// No password was passed.
		// Encrypt the file with a random session key and write the key
		// to a file. 

		//---------------------------------------------------------------
		// Create a random session key. 

		if(CryptGenKey(
			hCryptProv, 
			ENCRYPT_ALGORITHM, 
			KEYLENGTH | CRYPT_EXPORTABLE, 
			&hKey))
		{
			printf("A session key has been created. \n");
		} 
		else
		{
			HandleError("Error during CryptGenKey. \n"); 
		}
		//--------------------------------------------------------------- 
		// Get handle to the encrypter's exchange public key. 

		if(CryptGetUserKey(
			hCryptProv, 
			AT_KEYEXCHANGE, 
			&hXchgKey))
		{
			printf("The user public key has been retrieved. \n");
		}
		else
		{ 
			HandleError("User public key is not available \
						and may not exist."); 
		}
		//---------------------------------------------------------------
		// Determine size of the key BLOB, and allocate memory. 

		if(CryptExportKey(
			hKey, 
			hXchgKey, 
			SIMPLEBLOB, 
			0, 
			NULL, 
			&dwKeyBlobLen))
		{
			printf("The key BLOB is %d bytes long. \n",dwKeyBlobLen);
		}
		else
		{  
			HandleError("Error computing BLOB length! \n");
		}
		if(pbKeyBlob =(BYTE *)malloc(dwKeyBlobLen))
		{ 
			printf("Memory is allocated for the key BLOB. \n");
		}
		else
		{ 
			HandleError("Out of memory. \n"); 
		}
		//---------------------------------------------------------------
		// Encrypt and export session key into a simple key BLOB. 

		if(CryptExportKey(
			hKey, 
			hXchgKey, 
			SIMPLEBLOB, 
			0, 
			pbKeyBlob, 
			&dwKeyBlobLen))
		{
			printf("The key has been exported. \n");
		} 
		else
		{
			HandleError("Error during CryptExportKey!\n");
		} 
		//---------------------------------------------------------------
		// Release key exchange key handle. 
		CryptDestroyKey(hXchgKey); 
		hXchgKey = 0; 

		//---------------------------------------------------------------
		// Write size of key BLOB to destination file. 

		fwrite(&dwKeyBlobLen, sizeof(DWORD), 1, hDestination); 
		if(ferror(hDestination))
		{ 
			HandleError("Error writing header.");
		}
		else
		{
			printf("A file header has been written. \n");
		}
		//--------------------------------------------------------------
		// Write key BLOB to destination file. 

		fwrite(pbKeyBlob, 1, dwKeyBlobLen, hDestination); 
		if(ferror(hDestination))
		{ 
			HandleError("Error writing header");
		}
		else
		{
			printf("The key BLOB has been written to the file. \n");
		}
	} 
	else 
	{ 
		//--------------------------------------------------------------------
		// The file will be encrypted with a session key derived from a
		// password.
		// The session key will be recreated when the file is decrypted
		// only if the password used to create the key is available. 

		//--------------------------------------------------------------------
		// Create a hash object. 

		if(CryptCreateHash(
			hCryptProv, 
			CALG_MD5, 
			0, 
			0, 
			&hHash))
		{
			printf("A hash object has been created. \n");
		}
		else
		{ 
			HandleError("Error during CryptCreateHash!\n");
		}  
		//--------------------------------------------------------------------
		// Hash the password. 

		if(CryptHashData(
			hHash, 
			(BYTE *)szPassword, 
			strlen(szPassword), 
			0))
		{
			printf("The password has been added to the hash. \n");
		}
		else
		{
			HandleError("Error during CryptHashData. \n"); 
		}
		//--------------------------------------------------------------------
		// Derive a session key from the hash object. 

		if(CryptDeriveKey(
			hCryptProv, 
			ENCRYPT_ALGORITHM, 
			hHash, 
			KEYLENGTH, 
			&hKey))
		{
			printf("An encryption key is derived from the password hash. \n"); 
		}
		else
		{
			HandleError("Error during CryptDeriveKey!\n"); 
		}
		//--------------------------------------------------------------------
		// Destroy the hash object. 

		CryptDestroyHash(hHash); 
		hHash = 0; 
	} 
	//--------------------------------------------------------------------
	//  The session key is now ready. If it is not a key derived from a 
	//  password, the session key encrypted with the encrypter's private 
	//  key has been written to the destination file.

	//--------------------------------------------------------------------
	// Determine number of bytes to encrypt at a time. 
	// This must be a multiple of ENCRYPT_BLOCK_SIZE.
	// ENCRYPT_BLOCK_SIZE is set by a #define statement.

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE; 

	//--------------------------------------------------------------------
	// Determine the block size. If a block cipher is used, 
	// it must have room for an extra block. 

	if(ENCRYPT_BLOCK_SIZE > 1) 
		dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE; 
	else 
		dwBufferLen = dwBlockLen; 

	//--------------------------------------------------------------------
	// Allocate memory. 
	if(pbBuffer = (BYTE *)malloc(dwBufferLen))
	{
		printf("Memory has been allocated for the buffer. \n");
	}
	else
	{ 
		HandleError("Out of memory. \n"); 
	}
	//--------------------------------------------------------------------
	// In a do loop, encrypt the source file and write to the source file. 

	do 
	{ 

		//--------------------------------------------------------------------
		// Read up to dwBlockLen bytes from the source file. 
		dwCount = fread(pbBuffer, 1, dwBlockLen, hSource); 
		if(ferror(hSource))
		{ 
			HandleError("Error reading plaintext!\n");
		}

		//--------------------------------------------------------------------
		// Encrypt data. 
		if(!CryptEncrypt(
			hKey, 
			0, 
			feof(hSource), 
			0, 
			pbBuffer, 
			&dwCount, 
			dwBufferLen))
		{ 
			HandleError("Error during CryptEncrypt. \n"); 
		} 

		//--------------------------------------------------------------------
		// Write data to the destination file. 

		fwrite(pbBuffer, 1, dwCount, hDestination); 
		if(ferror(hDestination))
		{ 
			HandleError("Error writing ciphertext.");
		}

	} 
	while(!feof(hSource)); 
	//--------------------------------------------------------------------
	//  End the do loop when the last block of the source file has been
	//  read, encrypted, and written to the destination file.

	//--------------------------------------------------------------------
	// Close files.

	if(hSource) 
		fclose(hSource); 
	if(hDestination) 
		fclose(hDestination); 

	//--------------------------------------------------------------------
	// Free memory. 

	if(pbBuffer) 
		free(pbBuffer); 

	//--------------------------------------------------------------------
	// Destroy session key. 

	if(hKey) 
		CryptDestroyKey(hKey); 

	//--------------------------------------------------------------------
	// Release key exchange key handle. 

	if(hXchgKey) 
		CryptDestroyKey(hXchgKey); 

	//--------------------------------------------------------------------
	// Destroy hash object. 

	if(hHash) 
		CryptDestroyHash(hHash); 

	//--------------------------------------------------------------------
	// Release provider handle. 

	if(hCryptProv) 
		CryptReleaseContext(hCryptProv, 0);
	return(TRUE); 
} // End of Encryptfile
