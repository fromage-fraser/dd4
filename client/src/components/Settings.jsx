import React, { useState, useEffect } from 'react';
import './Settings.css';

/**
 * Settings component - Manages server connection profiles
 * 
 * Intent: Provide user-configurable server profiles with locked Dev/Prod defaults.
 * Allows up to 2 custom profiles with Test Connection functionality.
 * Blocks profile switching while connected to prevent mid-session server changes.
 * 
 * Profiles stored in localStorage with keys:
 * - dd4_server_profiles: Array of profile objects
 * - dd4_active_profile: Currently active profile ID
 */
function Settings({ profiles, activeProfile, onSave, onClose, connected }) {
  const [localProfiles, setLocalProfiles] = useState([...profiles]);
  const [selectedId, setSelectedId] = useState(activeProfile?.id);
  const [editMode, setEditMode] = useState(null); // null, 'add', or profile.id
  const [formData, setFormData] = useState({
    name: '',
    wsUrl: '',
    description: ''
  });
  const [errors, setErrors] = useState({});
  const [testingConnection, setTestingConnection] = useState(false);
  const [testResult, setTestResult] = useState(null);

  // Count custom (non-locked) profiles
  const customProfileCount = localProfiles.filter(p => !p.isLocked).length;
  const canAddCustom = customProfileCount < 2;

  // Handle form input changes
  const handleInputChange = (field, value) => {
    setFormData(prev => ({ ...prev, [field]: value }));
    // Clear error for this field
    setErrors(prev => ({ ...prev, [field]: null }));
  };

  // Validate WebSocket URL format
  const validateUrl = (url) => {
    console.log('[validateUrl] Input URL:', url);
    if (!url) return 'WebSocket URL is required';
    if (!url.startsWith('ws://') && !url.startsWith('wss://')) {
      console.log('[validateUrl] Failed protocol check');
      return 'URL must start with ws:// or wss://';
    }
    
    // Basic validation of host:port pattern
    // Don't use URL constructor as it doesn't support ws:// in all environments
    const match = url.match(/^wss?:\/\/([a-zA-Z0-9.-]+|\[[0-9a-fA-F:]+\])(?::(\d+))?(?:\/.*)?$/);
    if (!match) {
      console.log('[validateUrl] Regex match failed');
      return 'Invalid WebSocket URL format';
    }
    
    // Validate port if present
    if (match[2]) {
      const port = parseInt(match[2]);
      if (port < 1 || port > 65535) {
        console.log('[validateUrl] Invalid port:', port);
        return 'Port must be between 1 and 65535';
      }
    }
    
    console.log('[validateUrl] Valid URL');
    return null;
  };

  // Validate form data
  const validateForm = () => {
    const newErrors = {};
    
    if (!formData.name.trim()) {
      newErrors.name = 'Profile name is required';
    } else if (formData.name.length > 50) {
      newErrors.name = 'Name must be 50 characters or less';
    } else {
      // Check name uniqueness (excluding current profile if editing)
      const duplicate = localProfiles.find(p => 
        p.name.toLowerCase() === formData.name.toLowerCase() && 
        p.id !== editMode
      );
      if (duplicate) {
        newErrors.name = 'Profile name must be unique';
      }
    }
    
    const urlError = validateUrl(formData.wsUrl);
    if (urlError) {
      newErrors.wsUrl = urlError;
    }
    
    setErrors(newErrors);
    return Object.keys(newErrors).length === 0;
  };

  // Start adding new profile
  const handleAddProfile = () => {
    setEditMode('add');
    setFormData({
      name: '',
      wsUrl: 'wss://',
      description: ''
    });
    setErrors({});
    setTestResult(null);
  };

  // Start editing existing profile
  const handleEditProfile = (profile) => {
    if (profile.isLocked) return;
    setEditMode(profile.id);
    setFormData({
      name: profile.name,
      wsUrl: profile.wsUrl,
      description: profile.description || ''
    });
    setErrors({});
    setTestResult(null);
  };

  // Save profile (add or edit)
  const handleSaveProfile = () => {
    if (!validateForm()) return;

    if (editMode === 'add') {
      // Add new profile
      const newProfile = {
        id: `custom_${Date.now()}`,
        name: formData.name.trim(),
        wsUrl: formData.wsUrl.trim(),
        description: formData.description.trim(),
        isLocked: false,
        createdAt: Date.now()
      };
      setLocalProfiles([...localProfiles, newProfile]);
    } else {
      // Edit existing profile
      setLocalProfiles(localProfiles.map(p => 
        p.id === editMode 
          ? { ...p, name: formData.name.trim(), wsUrl: formData.wsUrl.trim(), description: formData.description.trim() }
          : p
      ));
    }

    setEditMode(null);
    setFormData({ name: '', wsUrl: '', description: '' });
    setTestResult(null);
  };

  // Cancel add/edit
  const handleCancelEdit = () => {
    setEditMode(null);
    setFormData({ name: '', wsUrl: '', description: '' });
    setErrors({});
    setTestResult(null);
  };

  // Delete custom profile
  const handleDeleteProfile = (profileId) => {
    const profile = localProfiles.find(p => p.id === profileId);
    if (!profile || profile.isLocked) return;

    if (!confirm(`Delete profile "${profile.name}"?`)) return;

    // If deleting active profile, switch to Dev
    let newSelectedId = selectedId;
    if (profileId === selectedId) {
      const devProfile = localProfiles.find(p => p.isLocked && p.name === 'Dev');
      newSelectedId = devProfile?.id || localProfiles[0].id;
    }

    setLocalProfiles(localProfiles.filter(p => p.id !== profileId));
    setSelectedId(newSelectedId);
  };

  // Test WebSocket connection
  const handleTestConnection = async () => {
    const urlError = validateUrl(formData.wsUrl);
    if (urlError) {
      setErrors({ wsUrl: urlError });
      return;
    }

    setTestingConnection(true);
    setTestResult(null);

    // Check if running in Capacitor (native app)
    const isNative = window.Capacitor?.isNativePlatform();
    console.log('[Test] Running in native app:', isNative);
    console.log('[Test] Window location:', window.location.href);
    console.log('[Test] Testing URL:', formData.wsUrl.trim());

    try {
      const ws = new WebSocket(formData.wsUrl.trim());
      
      const timeout = setTimeout(() => {
        ws.close();
        setTestingConnection(false);
        setTestResult({ success: false, message: 'Connection timeout' });
      }, 5000);

      ws.onopen = () => {
        clearTimeout(timeout);
        ws.close();
        setTestingConnection(false);
        setTestResult({ success: true, message: 'Connection successful!' });
      };

      ws.onerror = (event) => {
        console.log('[Test] WebSocket error:', event);
        clearTimeout(timeout);
        setTestingConnection(false);
        setTestResult({ success: false, message: 'Connection failed' });
      };
    } catch (error) {
      console.log('[Test] WebSocket constructor error:', error);
      setTestingConnection(false);
      // Show actual error - likely Mixed Content from browser
      const message = error.message || 'Failed to create WebSocket';
      setTestResult({ success: false, message });
    }
  };

  // Save and apply changes
  const handleSave = () => {
    const activeProf = localProfiles.find(p => p.id === selectedId);
    onSave(localProfiles, selectedId);
    onClose();
  };

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="settings-modal-content" onClick={(e) => e.stopPropagation()}>
        <div className="settings-modal-header">
          <h2>Server Profiles</h2>
          <button className="close-button" onClick={onClose}>✕</button>
        </div>
        
        <div className="settings-modal-body">
          {/* Left Panel - Profile List */}
          <div className="settings-profile-list">
            <h3>Profiles</h3>
            
            {connected && (
              <div className="settings-warning">
                <span>⚠️</span>
                <p>Disconnect to switch servers</p>
              </div>
            )}
            
            <div className="settings-profiles">
              {localProfiles.map(profile => (
                <div 
                  key={profile.id}
                  className={`settings-profile-item ${profile.id === selectedId ? 'active' : ''} ${connected && profile.id !== selectedId ? 'disabled' : ''}`}
                >
                  <div className="settings-profile-select">
                    <input
                      type="radio"
                      name="profile"
                      value={profile.id}
                      checked={profile.id === selectedId}
                      onChange={() => !connected && setSelectedId(profile.id)}
                      disabled={connected && profile.id !== selectedId}
                    />
                    <div className="settings-profile-info">
                      <div className="settings-profile-name">
                        {profile.isLocked && <span className="lock-icon">🔒</span>}
                        {profile.name}
                      </div>
                      <div className="settings-profile-url">{profile.wsUrl}</div>
                      {profile.description && (
                        <div className="settings-profile-desc">{profile.description}</div>
                      )}
                    </div>
                  </div>
                  
                  {!profile.isLocked && (
                    <div className="settings-profile-actions">
                      <button 
                        className="btn-icon"
                        onClick={() => handleEditProfile(profile)}
                        title="Edit profile"
                      >
                        ✏️
                      </button>
                      <button 
                        className="btn-icon"
                        onClick={() => handleDeleteProfile(profile.id)}
                        title="Delete profile"
                        disabled={profile.id === selectedId && connected}
                      >
                        🗑️
                      </button>
                    </div>
                  )}
                </div>
              ))}
            </div>

            <button 
              className="btn-add-profile"
              onClick={handleAddProfile}
              disabled={!canAddCustom || editMode !== null}
            >
              + Add Custom Profile
            </button>
            {!canAddCustom && (
              <p className="settings-limit-note">Maximum 2 custom profiles</p>
            )}
          </div>

          {/* Right Panel - Profile Form */}
          <div className="settings-profile-form">
            {editMode ? (
              <>
                <h3>{editMode === 'add' ? 'New Profile' : 'Edit Profile'}</h3>
                
                <div className="settings-form-group">
                  <label htmlFor="profile-name">Profile Name *</label>
                  <input
                    id="profile-name"
                    type="text"
                    value={formData.name}
                    onChange={(e) => handleInputChange('name', e.target.value)}
                    placeholder="My Server"
                    maxLength={50}
                    className={errors.name ? 'error' : ''}
                  />
                  {errors.name && <span className="error-message">{errors.name}</span>}
                </div>

                <div className="settings-form-group">
                  <label htmlFor="profile-url">WebSocket URL *</label>
                  <input
                    id="profile-url"
                    type="text"
                    value={formData.wsUrl}
                    onChange={(e) => handleInputChange('wsUrl', e.target.value)}
                    placeholder="wss://192.168.1.100:8080"
                    className={errors.wsUrl ? 'error' : ''}
                  />
                  {errors.wsUrl && <span className="error-message">{errors.wsUrl}</span>}
                  <span className="input-hint">Format: ws://hostname:port or wss://hostname:port</span>
                </div>

                <div className="settings-form-group">
                  <label htmlFor="profile-description">Description (optional)</label>
                  <textarea
                    id="profile-description"
                    value={formData.description}
                    onChange={(e) => handleInputChange('description', e.target.value)}
                    placeholder="Local development server"
                    rows={3}
                    maxLength={200}
                  />
                </div>

                <div className="settings-form-actions">
                  <button 
                    className="btn-test"
                    onClick={handleTestConnection}
                    disabled={testingConnection || !formData.wsUrl}
                  >
                    {testingConnection ? 'Testing...' : 'Test Connection'}
                  </button>
                  
                  {testResult && (
                    <div className={`test-result ${testResult.success ? 'success' : 'error'}`}>
                      {testResult.success ? '✓' : '✗'} {testResult.message}
                    </div>
                  )}
                </div>

                <div className="settings-form-buttons">
                  <button className="btn-save" onClick={handleSaveProfile}>
                    Save Profile
                  </button>
                  <button className="btn-cancel" onClick={handleCancelEdit}>
                    Cancel
                  </button>
                </div>
              </>
            ) : (
              <div className="settings-empty-state">
                <p>Select a profile to view details or add a new custom profile.</p>
                <div className="settings-help">
                  <h4>About Profiles</h4>
                  <p>🔒 <strong>Locked profiles</strong> (Dev, Prod) cannot be edited or deleted.</p>
                  <p>✏️ <strong>Custom profiles</strong> can be created, edited, and deleted.</p>
                  <p>⚠️ You must disconnect before switching to a different server.</p>
                </div>
              </div>
            )}
          </div>
        </div>

        <div className="settings-modal-footer">
          <button className="btn-apply" onClick={handleSave}>
            Apply & Close
          </button>
          <button className="btn-close" onClick={onClose}>
            Cancel
          </button>
        </div>
      </div>
    </div>
  );
}

export default Settings;
